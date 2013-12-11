#!/usr/bin/env python

import os

def read_stats_from_arduino():
    '''Reads information from the Arduino and returns stats as a tuple'''
    if __name__ == '__main__':
        print 'Watchdog barking!'

    return {'air temp'    : 70,
            'water temp'  : 60,
            'water level' : 2.3}

def take_picture():
    '''Take a picture from the camera and return a path to the filename'''
    os.system('raspistill -o ~/image.jpg')
    return '~/image.jpg'

def post_stats_to_twitter():
    stats = read_stats_from_arduino()
    stats_string = 'Air: {air temp}; Water: {water temp}; Level: {water level}'.format(**stats)
    if __name__ == '__main__':
        print 'Watchdog says: {}'.format(stats_string)
    image_path = take_picture()
    if __name__ == '__main__':
        print 'Posting to Twitter',
    os.system('t update "%s" -f %s' % (stats_string, image_path))
    
if __name__ == '__main__':
    print 'This the Garduino: Raspberry Pi Addition Edition'
    print 'Starting watchdog...',
    print 'Done!'
    print 'Watchdog staring...'
    post_stats_to_twitter()
    print 'Watchdog sleeping.  Program exit'
