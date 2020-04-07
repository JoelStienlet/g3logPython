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
g3logPython.debug("g3log set as backend for panda3d's logger")

from direct.directnotify.DirectNotify import DirectNotify
from direct.showbase.ShowBase import ShowBase
 
class MyApp(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)
        self.exitFunc=self.on_exit
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
        
    def on_exit(self):
        g3logPython.debug("exit called")
        pass

app = MyApp()
app.run() # https://docs.panda3d.org/1.10/python/programming/tasks-and-events/main-loop

#
# https://discourse.panda3d.org/t/how-to-close-and-restart-panda-cleanly/2537/6
#
#The normal way Panda3D handles exit is the following:
#
#    at startup Panda3D sets sys.exitfunc to ShowBase.exitfunc.
#    to shut down Panda3D the “normal” way the user calls sys.exit( )
#    sys.exit calls sys.exitfunc( ) before destroying the interpreter, which is ShowBase.exitfunc.
#    ShowBase.exitfunc handles shutting down the main window etc.
#
#So you could try to call base.exitfunc( ) yourself. Should close the Panda3D window and free resources.
#
#



