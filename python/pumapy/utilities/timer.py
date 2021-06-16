import time


class Timer:
    current = 0

    def __init__(self):
        self.current = time.perf_counter()

    def start(self):
        self.current = time.perf_counter()

    def reset(self):
        self.current = time.perf_counter()

    def elapsed(self):
        return time.perf_counter() - self.current

    def print_elapsed(self, msg=""):
        print(msg, (time.perf_counter() - self.current))
