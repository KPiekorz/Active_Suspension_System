from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT as NavigationToolbar
from PyQt5 import QtCore, QtGui, QtWidgets

import sys
import matplotlib
import os
import socket
import sys

matplotlib.use('Qt5Agg')

class UDPWorker(QtCore.QObject):
    dataChanged = QtCore.pyqtSignal(str)

    def __init__(self, parent=None):
        super(UDPWorker, self).__init__(parent)
        self.server_start = False

    @QtCore.pyqtSlot()
    def start(self):
        print('Hello World!')
        # self.server_start = True
        # ip = "192.168.1.4"
        # port = 515
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # self.sock.bind((ip,port))
        # self.process()

    def process(self):
        pass
        # while self.server_start:
        #     data, addr = self.sock.recvfrom(1024)
        #     self.dataChanged.emit(str(data))

class UDPWidget(QtWidgets.QWidget):
    started = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(UDPWidget, self).__init__(parent)
        btn = QtWidgets.QPushButton("Start Udp server")
        btn.clicked.connect(self.started)

        # self.lst = QtWidgets.QListWidget()

        layout = QtWidgets.QVBoxLayout(self)
        # layout.addWidget(QtWidgets.QLabel("Adaptive suspension symulator"))
        layout.addWidget(btn)
        # lay.addWidget(self.lst)

        self.setWindowTitle("Adaptive suspension symulator")

    @QtCore.pyqtSlot(str)
    def addItem(self, text):
        self.lst.insertItem(0, text)

if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    w = UDPWidget()
    worker = UDPWorker()
    thread = QtCore.QThread()
    thread.start()
    worker.moveToThread(thread)
    w.started.connect(worker.start)
    worker.dataChanged.connect(w.addItem)
    w.show()
    sys.exit(app.exec_())

# class MplCanvas(FigureCanvasQTAgg):

#     def __init__(self, parent=None, width=5, height=4, dpi=100):
#         fig = Figure(figsize=(width, height), dpi=dpi)
#         self.axes = fig.add_subplot(111)
#         super(MplCanvas, self).__init__(fig)


# class UDPWorker(QtCore.QObject):
#     dataChanged = QtCore.pyqtSignal(str)

#     def __init__(self, parent=None):
#         super(UDPWorker, self).__init__(parent)
#         self.server_start = False

#     @QtCore.pyqtSlot()
#     def start(self):
#         self.server_start = True
#         ip = '' # INADDR_ANY
#         port = 1100
#         self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#         self.sock.bind((ip, port))
#         self.process()

#     def process(self):

#         while (1):
#             print("Hello from process!")

#         # while self.server_start:
#         #     data, addr = self.sock.recvfrom(1024)
#         #     print("Data udp data received: " + data)

#             # self.dataChanged.emit(str(data))


# class MainWindow(QtWidgets.QMainWindow):
#     started = QtCore.pyqtSignal()

#     def __init__(self, *args, **kwargs):
#         super(MainWindow, self).__init__(*args, **kwargs)

#         sc = MplCanvas(self, width=5, height=4, dpi=100)
#         sc.axes.plot([0, 1, 2, 3, 4], [10, 1, 20, 3, 40])

#         # Create toolbar, passing canvas as first parament, parent (self, the MainWindow) as second.
#         toolbar = NavigationToolbar(sc, self)

#         layout = QtWidgets.QVBoxLayout()
#         layout.addWidget(toolbar)
#         layout.addWidget(sc)

#         # Create a placeholder widget to hold our toolbar and canvas.
#         widget = QtWidgets.QWidget()
#         widget.setLayout(layout)
#         self.setCentralWidget(widget)

#         self.show()

#     @QtCore.pyqtSlot(str)
#     def addItem(self, text):
#         # self.lst.insertItem(0, text)
#         print("Data udp data received: " + text)


# if __name__ == '__main__':
#     app = QtWidgets.QApplication(sys.argv)
#     w = MainWindow()

#     udp_worker = UDPWorker()
#     thread = QtCore.QThread()
#     thread.start()
#     udp_worker.moveToThread(thread)
#     w.started.connect(udp_worker.start)
#     udp_worker.dataChanged.connect(w.addItem)

#     sys.exit(app.exec_())


'''
class UDPWidget(QtWidgets.QWidget):
    started = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(UDPWidget, self).__init__(parent)
        btn = QtWidgets.QPushButton("Click Me")
        btn.clicked.connect(self.started)
        self.lst = QtWidgets.QListWidget()

        lay = QtWidgets.QVBoxLayout(self)
        lay.addWidget(QtWidgets.QLabel("udp receiver"))
        lay.addWidget(btn)
        lay.addWidget(self.lst)

        self.setWindowTitle("udp receive")

    @QtCore.pyqtSlot(str)
    def addItem(self, text):
        self.lst.insertItem(0, text)

if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    w = UDPWidget()
    worker = UDPWorker()
    thread = QtCore.QThread()
    thread.start()
    worker.moveToThread(thread)
    w.started.connect(worker.start)
    worker.dataChanged.connect(w.addItem)
    w.show()
    sys.exit(app.exec_())
'''
