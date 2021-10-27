import datetime
import os
import pumapy


def print_warning(warning_text):
    print(Colors.WARNING + "WARNING: " + warning_text + Colors.ENDC)


class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class Logger:
    def __init__(self):
        self.log = ""
        self.location = pumapy.settings['log_location']

        if not os.path.exists(self.location):
            try:
                os.mkdir(self.location)
            except PermissionError:
                self.location = ""
                print("\nInvalid log location. No log will be generated")


        now = datetime.datetime.now()
        month = str(now.month) if len(str(now.month)) == 2 else "0" + str(now.month)
        day = str(now.day) if len(str(now.day)) == 2 else "0" + str(now.day)
        hour = str(now.hour) if len(str(now.hour)) == 2 else "0" + str(now.hour)
        minute = str(now.minute) if len(str(now.minute)) == 2 else "0" + str(now.minute)
        second = str(now.second) if len(str(now.second)) == 2 else "0" + str(now.second)
        self.location = os.path.join(self.location, "pumapy_log_" + str(now.year) + '-' + month + '-' + day + "_" +
                                     hour + ';' + minute + ';' + second + ".txt")

    def log_section(self, name):
        self.log += "\n---------------------------- \n" + str(name) + "\n---------------------------- \n"

    def log_line(self, val):
        self.log += str(val) + "\n"

    def log_item(self, val):
        self.log += str(val) + " "

    def new_line(self):
        self.log += "\n"

    def log_bool(self, var_name, val):
        if val:
            self.log_line(str(var_name) + ": True")
        else:
            self.log_line(str(var_name) + ": False")

    def log_value(self, var_name, val):
        self.log_line(str(var_name) + ": " + str(val))

    def write_log(self):
        if self.location == "":
            return

        try:
            txt_file = open(self.location, "w")
            txt_file.write(self.log)
            txt_file.close()
        except PermissionError:
            self.location = ""
            print("\nInvalid log location. No log will be generated")
