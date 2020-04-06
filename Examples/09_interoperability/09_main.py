#!/usr/bin/env python3 

import g3logPython
import cppP3Dcalls

logger = g3logPython.get_ifaceLogWorker(False)
colorTermSink = logger.ClrTermSinks.new_Sink("color term")
g3logPython.debug("g3log logger has started")
g3logPython.debug("don't forget to edit /etc/Config.prc")
# TODO: add python builtin logger interoperability

from panda3d.core import Notify, OFileStream, Filename, MultiplexStream

cppP3Dcalls.set_g3log_as_panda3d_backend()

from direct.directnotify.DirectNotify import DirectNotify
from direct.showbase.ShowBase import ShowBase
 
class MyApp(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)
        
        # Load the environment model.
        self.scene = self.loader.loadModel("models/environment")
        # Reparent the model to render.
        self.scene.reparentTo(self.render)
        # Apply scale and position transforms on the model.
        self.scene.setScale(0.25, 0.25, 0.25)
        self.scene.setPos(-8, 42, 0)
        
        self.accept('a', self.printRepeat)
        
        self.py3notify = DirectNotify().newCategory("Python3_main")
        
    def printRepeat(self):
        #print("got 'a'")
        self.py3notify.warning("got 'a'")

app = MyApp()
app.run()
