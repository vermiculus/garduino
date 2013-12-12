#!/usr/bin/env python

import os

def increment_index(directory):
    if not os.path.exists(directory):
        if __name__ == '__main__':
            print "The Watchdog couldn't find the directory you wanted to use for images!"
            print "Don't worry; he'll make it for you."
        os.system('mkdir -p {} && echo -1 > {}/index'.format(directory, directory))

    f = open('{}/index'.format(directory), 'r')
    lines = f.readlines()
    f.close()
    
    index_line = lines[0]
    index = int(index_line) + 1

    f = open('{}/index'.format(directory), 'w+')
    f.write(str(index))
    f.close()

    return index

def update_iteration(directory, max_images=30):
    '''Keeps a rolling history of images; returns next image's index'''
    index = increment_index(directory)

    images = [img for img in os.listdir(directory)
                   if img.startswith('image')
                  and img.endswith('jpg')]

    if len(images) > max_images:
        os.remove('{}/{}'.format(directory, sorted(images)[0]))

    return index

def read_stats_from_arduino():
    '''Reads information from the Arduino and returns stats as a tuple'''
    if __name__ == '__main__':
        print 'Watchdog barking!'

    import serial

    data = 'null'
    while data == 'null':
        try:
            arduino = serial.Serial('/dev/ttyACM0', 9600)
            temp = arduino.readline()
            if temp[0] is '!':
                data = temp
        except:
            print 'nothing new'
        finally:
            try:
                arduino.close()
            except:
                print 'Did you plug in the Arduino?'
                data = '!50 60'

    print data
    parts = data[1:].split()

    return {'air temp'    : int(parts[0]),
            'water temp'  : int(parts[1])}

def take_picture(directory):
    '''Take a picture from the camera and return a path to the filename'''
    new_index = update_iteration(directory)

    image_path = '{}/image{}.jpg'.format(directory, new_index)

    os.system('raspistill -t 0 -o {}'.format(image_path))
    return image_path

def post_stats_to_twitter(image_directory):
    stats = read_stats_from_arduino()
    stats_string = 'Air: {air temp} C; Water: {water temp} C'.format(**stats)
    if __name__ == '__main__':
        print 'Watchdog says: {}'.format(stats_string)
    image_path = take_picture(image_directory)
    if __name__ == '__main__':
        print 'Posting to Twitter...',
    os.system('t update "%s" -f %s' % (stats_string, image_path))
    
if __name__ == '__main__':
    import time
    print 'This the Garduino: Raspberry Pi Addition Edition'
    print 'Starting watchdog...',
    image_dir='/home/pi/images'
    print 'Done!'
    print 'Watchdog staring...'
#    while True:
    post_stats_to_twitter(image_dir)
    print 'Watchdog is proud!'
    time.sleep(5) # sleep five seconds
    print 'Watchdog sleeping.  Program exit'
