import time
import subprocess
import argparse
import os

DEFAULT_INTERVAL_US = 10000
DEFAULT_NUM_FLOWS = 10

def parse_options():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--interval",
        help="the interval to run short flow, (default: " + str(DEFAULT_INTERVAL_US) + ")",
        dest="interval_us",
        type=int,
        default=DEFAULT_INTERVAL_US
    )

    parser.add_argument(
        "-n",
        "--the number of flows",
        help="the number of short flows to run, (default: " + str(DEFAULT_NUM_FLOWS) + ")",
        dest="num_flow",
        type=int,
        default=DEFAULT_INTERVAL_US
    )

    options = parser.parse_args()
    return options

def get_usleep_overhead_us(num):
    accum_time = 0
    for i in range(num):
        start_time = time.time()
        ret_code = subprocess.call('usleep(0)',shell=True)
        end_time = time.time()
        spent_time = end_time-start_time
        accum_time += spent_time
    return float(accum_time * 1000000)/num

def run_interval_short_flow(interval_us, num_flow):
    # usleep_overhead_us = get_usleep_overhead_us(100)
    last_time = time.time()
    last_time_us = last_time * 1000000
    current_flow = 1
    while (current_flow <= num_flow):
        current_time = time.time()
        current_time_us = current_time * 1000000
        # current_time_us -= usleep_overhead_us
        interval = current_time_us - last_time_us
        if (interval > interval_us):
            print "!!! current time is %s \n" %(str(current_time_us))
            ret_val = os.system("bin/client -fi -v -c conf/client_to_h1.txt -n 1 -l flows.txt -b 1000 -r script/result.py")
            last_time_us = time.time() * 1000000
            current_flow += 1

def main():
    options = parse_options()
    num_flows = options.num_flow
    interval_us = options.interval_us

    run_interval_short_flow(interval_us, num_flows)

if __name__ == '__main__':
    main()