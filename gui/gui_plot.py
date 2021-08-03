from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT as NavigationToolbar
from PyQt5 import QtCore, QtGui, QtWidgets

import sys
import matplotlib
import os
import socket
import sys
import random

matplotlib.use('Qt5Agg')


class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(MplCanvas, self).__init__(fig)

class UDPServer(QtCore.QObject):

    def __init__(self, parent=None):
        super(UDPServer, self).__init__(parent)
        self.server_start = False

    @QtCore.pyqtSlot()
    def init_server(self):
        print("Udp server starting...")
        self.server_start = True
        ip = ''
        port = 1100
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((ip,port))
        self.receive_server_data()

    def receive_server_data(self):
        print("Upd server has started...")
        while self.server_start:
            data, addr = self.sock.recvfrom(1024)
            print("Udp server received data:", end='')
            print(data)

class SerialReader(QtCore.QObject):

    def __init__(self, parent=None):
        super(SerialReader, self).__init__(parent)

    @QtCore.pyqtSlot()
    def read_serial(self):
        while(1):
            text = input("Enter :> ");
            print(text)

class GUI(QtWidgets.QMainWindow):
    signal_start_udp_server = QtCore.pyqtSignal()
    signal_start_serial_reader = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(GUI, self).__init__(parent)

        # Set box parameters
        self.setWindowTitle("Adaptive suspension symulator")
        self.setGeometry(100, 100, 600, 800)

        # Close gui
        button_close_gui = QtWidgets.QPushButton("Close")
        button_close_gui.clicked.connect(self.close)

        # Setup init udp server buton
        button_start_udp_server = QtWidgets.QPushButton("Start UDP server")
        button_start_udp_server.clicked.connect(self.signal_start_udp_server)

        # Setup serial reader button
        button_start_serial_reader = QtWidgets.QPushButton("Start serial reader")
        button_start_serial_reader.clicked.connect(self.signal_start_serial_reader)

        # Set plot paramaters
        self.plot_control = MplCanvas(self, width=5, height=4, dpi=100)
        self.plot_model = MplCanvas(self, width=5, height=4, dpi=100)
        self.update_plot()

        # Create toolbar, passing canvas as first parament, parent (self, the MainWindow) as second.
        toolbar_control = NavigationToolbar(self.plot_control, self)
        toolbar_model = NavigationToolbar(self.plot_model, self)

        # Setup box layout
        box_layout = QtWidgets.QVBoxLayout()
        box_layout.addWidget(button_close_gui)
        box_layout.addWidget(button_start_udp_server)
        box_layout.addWidget(button_start_serial_reader)
        box_layout.addWidget(toolbar_control)
        box_layout.addWidget(self.plot_control)
        box_layout.addWidget(toolbar_model)
        box_layout.addWidget(self.plot_model)

        # Create a placeholder widget to hold our toolbar and canvas.
        widget = QtWidgets.QWidget()
        widget.setLayout(box_layout)
        self.setCentralWidget(widget)

        # Show gui
        self.show()

        # Setup timers to update draw the plot
        self.timer = QtCore.QTimer()
        self.timer.setInterval(500)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start()

    def update_plot(self):
        # clear plot
        self.plot_control.axes.cla()
        # set new plot axes values
        self.plot_control.axes.plot([0, 1, 2, 3, 4], [10, 1, random.randint(0, 10), 3, 40])
        # Trigger the canvas to update and redraw.
        self.plot_control.draw()

        # clear plot
        self.plot_model.axes.cla()
        # set new plot axes values
        self.plot_model.axes.plot([0, 1, 2, 3, 4], [10, 1, random.randint(0, 10), 3, random.randint(0, 10)])
        # Trigger the canvas to update and redraw.
        self.plot_model.draw()

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
