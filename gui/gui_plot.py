from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT as NavigationToolbar
from PyQt5 import QtCore, QtGui, QtWidgets

import sys
import matplotlib
import os
import socket
import sys
import time

matplotlib.use('Qt5Agg')

class UDPServer(QtCore.QObject):

    def __init__(self, parent=None):
        super(UDPServer, self).__init__(parent)

    @QtCore.pyqtSlot()
    def init_server(self):
        print("Start")
        # self.server_start = True
        # ip = ''
        # port = 1100
        # self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # self.sock.bind((ip,port))
        # self.receive_server_data()

    def receive_server_data(self):
        while(1):
            time.sleep(1)
            print("process...")
        # while self.server_start:
        #     data, addr = self.sock.recvfrom(1024)
        #     self.dataChanged.emit(str(data))

class SerialReader(QtCore.QObject):

    def __init__(self, parent=None):
        super(SerialReader, self).__init__(parent)

    @QtCore.pyqtSlot()
    def read_serial(self):
        while(1):
            text = input("Enter :> ");
            print(text)

class GUI(QtWidgets.QWidget):
    signal_start_udp_server = QtCore.pyqtSignal()
    signal_start_serial_reader = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(GUI, self).__init__(parent)

        # Set box parameters
        self.setWindowTitle("Adaptive suspension symulator")
        self.setGeometry(100, 100, 600, 400)

        # Close gui
        button_close_gui = QtWidgets.QPushButton("Close")
        button_close_gui.clicked.connect(self.close)

        # Setup init udp server buton
        button_start_udp_server = QtWidgets.QPushButton("Start UDP server")
        button_start_udp_server.clicked.connect(self.signal_start_udp_server)

        # Setup serial reader button
        button_start_serial_reader = QtWidgets.QPushButton("Start serial reader")
        button_start_serial_reader.clicked.connect(self.signal_start_serial_reader)

        # Setup box layout
        box_layout = QtWidgets.QVBoxLayout(self)
        box_layout.addWidget(button_close_gui)
        box_layout.addWidget(button_start_udp_server)
        box_layout.addWidget(button_start_serial_reader)

        # Show gui
        self.show()

    def process_udp_server_data():
        print("Process udp server data...")

if __name__ == '__main__':

    app = QtWidgets.QApplication(sys.argv)
    gui = GUI()

    upd_server = UDPServer()
    thread_upd_server = QtCore.QThread()
    thread_upd_server.start()
    upd_server.moveToThread(thread_upd_server)

    serial_reader = SerialReader()
    thread_serial_reader = QtCore.QThread()
    thread_serial_reader.start()
    serial_reader.moveToThread(thread_serial_reader)

    gui.signal_start_udp_server.connect(upd_server.init_server)
    gui.signal_start_serial_reader.connect(serial_reader.read_serial)

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
