import argparse
import subprocess
import os
import sys
import signal
import time
import re

DEFAULT_MONITOR_INTERFACE = 'eno2'
DEFAULT_QUEUE_SIZE = 800
DEFAULT_DURATION = 10
DEFAULT_RESOLUTION = 0.01
DEFAULT_LOG_FILE = '/home/hcy/queue_size.log'

def parse_options():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--interface",
        help="the interface to monitor, (default: " + DEFAULT_MONITOR_INTERFACE + ")",
        dest="interface",
        default=DEFAULT_MONITOR_INTERFACE
    )

    # parser.add_argument(
    #     "-l",
    #     "--queue length",
    #     help="the queue length of the interface, (default: " + str(DEFAULT_QUEUE_SIZE) + ")",
    #     dest="queue_length",
    #     type=float,
    #     default=DEFAULT_QUEUE_SIZE
    # )

    parser.add_argument(
        "-d",
        "--duration",
        help="the duration time of monitoring, (default: "+str(DEFAULT_DURATION)+")",
        dest="duration",
        type=float,
        default=DEFAULT_DURATION
    )

    parser.add_argument(
        "-r",
        "--resolution",
        help="the resolution of monitoring, (default: "+str(DEFAULT_RESOLUTION)+")",
        dest="resolution",
        type=float,
        default=DEFAULT_RESOLUTION
    )

    parser.add_argument(
        "-w",
        "--write-to-file",
        help="the file to store information, (default: "+DEFAULT_LOG_FILE+")",
        dest="logfile",
        default=DEFAULT_LOG_FILE
    )

    options = parser.parse_args()
    return options

def pre_logging(handler):
    handler.write("Time " + "Size " + "\n")

def post_logging(handler):
    handler.close()

def log_samples(handler, input):
    '''specify the value to log'''
    output = ''
    current_time = time.time()
    tmp_data = input.strip().split(" ")
    ix = tmp_data.index('backlog')
    qlen = tmp_data[ix+2]
    m = re.match(r'\d+', qlen)
    output += (str(current_time)+' '+str(m.group(0)))
    handler.write(output+'\n')

def monitor_queue_size(options):
    start_time = time.time()
    end_time = start_time + options.duration
    current_time = 0
    logfile_handler = open(options.logfile, 'w')
    pre_logging(logfile_handler)
    while current_time<end_time:
        current_time = time.time()
        out = subprocess.check_output('tc -p -s -d qdisc show dev '+options.interface, shell=True)
        log_samples(logfile_handler, out)
        time.sleep(options.resolution)
    post_logging(logfile_handler)

def main():
    options = parse_options()
    # set the queue length of the specific interface
    # ret_code = subprocess.call('ip link set '+options.interface+' qlen '+ str(options.queue_length), shell=True)
    monitor_queue_size(options)

if __name__ == "__main__":
    main()