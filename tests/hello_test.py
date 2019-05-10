from ctypes import cdll

lib = cdll.LoadLibrary('./libhello.so')

class Hello(object):
  def __init__(self):
    self.obj = lib.init()
  
  def curse_words(self):
    lib.say_it_all(self.obj)

  def get_array(self):
    print(lib.return_int(self.obj))
 
  def add_two(self, i):
    return lib.add_two(i)

hello = Hello()
hello.curse_words()
#hello.get_array()
test = 0
for j in range(0, 10):
  test = hello.add_two(test)
  print(test)
