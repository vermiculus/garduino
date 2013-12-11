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

    return {'air temp'    : 70,
            'water temp'  : 60,
            'water level' : 2.3}

def take_picture(directory):
    '''Take a picture from the camera and return a path to the filename'''
    new_index = update_iteration(directory)

    image_path = '{}/image{}.jpg'.format(directory, new_index)

    os.system('raspistill -o {}'.format(image_path))
    return image_path

def post_stats_to_twitter(image_directory):
    stats = read_stats_from_arduino()
    stats_string = 'Air: {air temp}; Water: {water temp}; Level: {water level}'.format(**stats)
    if __name__ == '__main__':
        print 'Watchdog says: {}'.format(stats_string)
    image_path = take_picture(image_directory)
    if __name__ == '__main__':
        print 'Posting to Twitter...',
    os.system('t update "%s" -f %s' % (stats_string, image_path))
    
if __name__ == '__main__':
    print 'This the Garduino: Raspberry Pi Addition Edition'
    print 'Starting watchdog...',
    image_dir='/home/pi/images'
    print 'Done!'
    print 'Watchdog staring...'
    post_stats_to_twitter(image_dir)
    print 'Watchdog sleeping.  Program exit'
