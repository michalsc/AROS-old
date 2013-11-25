# -*- coding: iso-8859-15 -*-

class Function:
    def __init__(self, buildenv):
        self.buildenv = buildenv
        self.directory = buildenv.get_variable("CURDIR")


class Output(Function):
    def __init__(self, buildenv, text):
        Function.__init__(self, buildenv)
        self.text = text


    def execute(self):
        text = self.buildenv.substitute(self.text)
        print text
        return True
