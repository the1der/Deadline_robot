
import face_recognition
import cv2
import numpy as np
import serial
import asyncio
import os
import json
import sys
from datetime import datetime

HEADERSIZE=10


if __name__ != '__main__':
    exit()



def send_message(msg, w):
    # s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    header= f'{len(msg):<10}'
    f_msg=header+str(msg)

    # s.connect((HOST,PORT))
    print(f_msg)
    w.write(bytes(f_msg,"utf-8"))

    # sw.recv(1)

def read_msg(r):
    msg=r.read(HEADERSIZE)
    msg_l=int(msg)
    print(f'New message with {msg_l} length')   
    msg=r.read(msg_l)
    # s.send(bytes("b","utf-8"))
    return msg.decode("utf-8")

def send_image(image,w):
    print("sending image: " + image)
    send_message(image, w)
    with open("../../Pictures/" + image, 'rb') as f:
        image_data = f.read() 
    header= f'{len(image_data):<{HEADERSIZE}}'
    print(header)
    w.write(bytes(header,"utf-8"))
    # s.connect((HOST,PORT))
    w.write(image_data)


def send_message_to_arduino(message:str):
    try:
        arduino.write(message.encode())
    except Exception as e:
        print(e)

def arduino_to_clinet(w):
    smth=arduino.readline()
    if len(smth)>0:
        w.send(smth)

def save_pic(frame):
    pics_directory="../../Pictures/"
    now = datetime.now()
    current_time = now.strftime("%m%d%Y_%H%M%S")
    cv2.imwrite(pics_directory+current_time+".png",frame)




async def client_handler_tcp(reader:asyncio.StreamReader, writer:asyncio.StreamWriter):
    message_size = 0
    message_size_bytes = await reader.read(4)
    message_size = message_size.from_bytes(message_size_bytes, 'little')
    message = (await reader.read(message_size)).decode()
    if message == 'start':
        await queue.put('start')
    elif message == 'stop':
        await recognition_queue.put('stop')
        await queue.put('stop')
    elif message == 'delete':
        print('deleting all images')
        if os.path.exists('images'):
            for image in os.listdir('images'):
                os.remove('images/' + image)

    elif 'config' in message:
        config = json.loads(message)
        print(config['config'])
        await queue.put(config['config'])


    elif ('image' in message):
        message = message.split('=')
        print("saving image: " + message[1])
        message_size_bytes = await reader.read(4)
        message_size = message_size.from_bytes(message_size_bytes, 'little')
        image = await reader.read(message_size)
        if not os.path.exists('images'):
            os.mkdir('images')
        with open("images/" + message[1], 'wb') as f:
            f.write(image)
        
    elif('getpics' in message):
            await recognition_queue.put('stop')
            await queue.put('stop')

            #get nbr of imgs
            _, _, files = next(os.walk("../../Pictures/"))
            nbr_of_images = len(files)
            #send 
            # send_message(str(nbr_of_images), writer)
            send_message(str(nbr_of_images), writer)

            #send imgs
            # send_image(files[0],clientsocket)
            for i in range(nbr_of_images):
                send_image(files[i],writer)
                # os.remove("../../Pictures/"+files[i])
    elif 'donepics' in message:
            _, _, files = next(os.walk("../../Pictures/"))
            nbr_of_images = len(files)
            for i in range(nbr_of_images):
                os.remove("../../Pictures/"+files[i])
    # elif ('logs' in message):
    #     global logs
    #     send_message(logs, writer)  
    #     logs = ""

    # writer.close()

    
async def tcp_server():
    server = await asyncio.start_server(client_handler_tcp, '0.0.0.0', 8000)
    async with server:
        print('server started')
        await server.serve_forever()





async def recognition_loop():
    global logs
    print('starting recognition loop')
    logs += "starting recognition loop\n"
    # This is a demo of running face recognition on live video from your webcam. It's a little more complicated than the
    # other example, but it includes some basic performance tweaks to make things run a lot faster:
    #   1. Process each video frame at 1/4 resolution (though still display it at full resolution)
    #   2. Only detect faces in every other frame of video.
        
    # PLEASE NOTE: This example requires OpenCV (the `cv2` library) to be installed only to read from your webcam.
    # OpenCV is *not* required to use the face_recognition library. It's only required if you want to run this
    # specific demo. If you have trouble installing it, try any of the other demos that don't require it instead.

    # Get a reference to webcam #0 (the default one)
    while recognition_queue.qsize() != 0:
        await recognition_queue.get()

    video_capture = None
    camera_found = False
    while True:
        if camera_found == True:
            break
        if recognition_queue.qsize() != 0:
            message = await recognition_queue.get()
            break
        # try all possible webcams
        for i in range(4):
            
            video_capture = cv2.VideoCapture(0)
            if video_capture is None or video_capture.isOpened():
                camera_found = True
                break
            print("failed to find camera")
            logs += "failed to find camera\n"
            await asyncio.sleep(0.1)
    print("Found camera")
    logs += "Found camera\n"
    # Load a sample picture and learn how to recognize it.
    known_face_names = []
    known_face_encodings = []
    for image_name in os.listdir('images'):

        image = face_recognition.load_image_file('images/' + image_name)
        face_encoding = face_recognition.face_encodings(image)[0]
        known_face_names.append(image_name.split('.')[0])
        known_face_encodings.append(face_encoding)

    # Initialize some variables
    face_locations = []
    face_encodings = []
    face_names = []
    process_this_frame = True


    # flushing queue

    i = 0

    while True:
        await asyncio.sleep(0.1)
        if recognition_queue.qsize() != 0:
            message = await recognition_queue.get()
            break
        


        # Grab a single frame of video
        ret, frame = video_capture.read()
        process_this_frame = not process_this_frame

        # Only process every other frame of video to save time
        if process_this_frame:
            i+=1
            if(i==19):
                print("saving...")
                save_pic(frame)
                i=0

            # Resize frame of video to 1/4 size for faster face recognition processing
            small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)

            # Convert the image from BGR color (which OpenCV uses) to RGB color (which face_recognition uses)
            rgb_small_frame = small_frame[:, :, ::-1]
            
            # Find all the faces and face encodings in the current frame of video
            face_locations = face_recognition.face_locations(rgb_small_frame)
            face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

            face_names = []
            for face_encoding in face_encodings:
                # See if the face is a match for the known face(s)
                name = "Unknown"

                # # If a match was found in known_face_encodings, just use the first one.
                # if True in matches:
                #     first_match_index = matches.index(True)
                #     name = known_face_names[first_match_index]

                # Or instead, use the known face with the smallest distance to the new face
                if len(known_face_encodings) != 0:
                    matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
                    face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
                    best_match_index = np.argmin(face_distances)
                    if matches[best_match_index]:
                        name = known_face_names[best_match_index]

                face_names.append(name)

            for name in face_names:
                if name in known_face_names:
                    print('found face: ' + name)
                else:
                    print('unknown face:', name)
                    send_message_to_arduino("f")
                    

            if len(face_names) == 0:
                print('no face')
           

    # Release handle to the webcam
    video_capture.release()
    print('stopping recognition loop')
    logs += "stopping recognition loop\n"

    return


async def main():
    global queue
    global recognition_queue
    global logs
    logs = ""

    # _, _, files = next(os.walk("../../Pictures/"))
    # nbr_of_images = len(files)
    # for i in range(nbr_of_images):
    #     os.remove("../../Pictures/"+files[i])
            
    queue = asyncio.Queue()
    recognition_queue = asyncio.Queue()

    recognition_task_created = False
    tcp_server_task = asyncio.create_task(tcp_server())
    config_ = {}
    # if os.path.exists('config.json'):
    #     with open('config.json') as f:
    #         config_ = json.load(f)
    #         arduino_message= str(config_['x'])+'$'+str(config_['y'])+'$'+str(config_['x0'])
    #         send_message_to_arduino(arduino_message)
    arduino_port = '/dev/ttyUSB0'
    if len(sys.argv) != 2:
        
        print('using default com: /dev/ttyUSB0')
        logs += 'using default com: /dev/ttyUSB0\n'
    else:
        arduino_port = sys.argv[1]

    global arduino
    while True:
        try:
            arduino = serial.Serial(arduino_port, 9600 , timeout = 0.1)
            break
        except:
            print(f"No ESP32 connected on port: {arduino_port}")
            logs += f"No ESP32 connected on port: {arduino_port}\n"
            await asyncio.sleep(0.1)
        
    print("esp32 found")
    logs += "esp32 found\n"



    while True:
        message = await queue.get()
        print("????")
        print(message)
        if message == 'start':
            if recognition_task_created:
                print('already started')
            else:
                recognition_task = asyncio.create_task(recognition_loop())
                recognition_task_created = True
                send_message_to_arduino('s')
                

        elif message == 'stop':
            if not recognition_task_created:
                print('task already stopped')
            else:
                recognition_task_created = False
                send_message_to_arduino('p')
 
        elif message == 'sendpics':
            pass

        elif type(message) == dict:
            config_ = message
            # send_message_to_arduino('configuration')
            with open('config.json', 'w') as f:
                json.dump(config_, f)
            

            

        else: 
            print("unknown command: " + message)




asyncio.run(main())