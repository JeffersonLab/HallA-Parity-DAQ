from ctypes import cdll

lib = cdll.LoadLibrary('./libhello.so')

class Hello(object):
  def __init__(self):
    self.obj = lib.init()
  
  def curse_words(self):
    lib.say_it_all(self.obj)

hello = Hello()
hello.curse_words()
