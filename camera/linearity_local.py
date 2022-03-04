#!/usr/bin/env python3

import os
import signal
import subprocess as sub
from random import random
from time import sleep

import numpy as np

def call(cmd):
    out = sub.run(cmd.split(), stdout=sub.PIPE, stderr=sub.PIPE)
    return out.stdout.decode(), out.stderr.decode()


class Watchdog(Exception):
    def __init__(self, time=5):
        self.time = time

    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handler)
        signal.alarm(self.time)

    def __exit__(self, type, value, traceback):
        signal.alarm(0)

    def handler(self, signum, frame):
        raise self


def capture(fname, gain=1, shutter=5000):
    cmd = (
        f"libcamera-still "
        f"-o image.jpg "
        f"--analoggain {gain} "
        f"--shutter {shutter} "
        f"--flush "
        f"--nopreview "
        f"--denoise off "
        f"--rawfull "
        f"--raw "
        f"--autofocus off "
        f"--awbgains 1,1 "
    )
    for i in range(10):
        try:
            with Watchdog(30):
                ans = call(cmd)
        except Watchdog:
            call("sudo reboot")
        break
    return ans


if __name__ == "__main__":
    shutters = sorted(
        (np.logspace(0, 6, 7, dtype=int) * [[1], [2], [5]]).flatten()
    )
    N = 10

    input("  - Remove the cap for data aquisition [ENTER] ")
    for ss in shutters:
        print(ss)
        for idx in range(N):
            sleep(random())
            capture(f"LINEARITY/{ss}_{idx}", shutter=ss)

    input("  - Put the cap on for a dark [ENTER] ")
    for ss in shutters:
        print(ss)
        for idx in range(N):
            capture(f"LINEARITY/DARKS/{ss}_{idx}", shutter=ss)
            sleep(random())

    print("\nDone.")
