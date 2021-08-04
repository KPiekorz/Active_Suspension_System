from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT as NavigationToolbar
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QLabel

import sys
import matplotlib
import socket
import sys
from random import randrange
import struct

matplotlib.use('Qt5Agg')

road = []
control_force = []
model_states = [1, 2, 3]

class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        super(MplCanvas, self).__init__(self.fig)

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
            print("Udp server received data: ", end='')
            b_x1 = data[0:4]
            f_x1 = struct.unpack('f', b_x1)
            b_x2 = data[4:8]
            f_x2 = struct.unpack('f', b_x2)
            b_x3 = data[8:12]
            f_x3 = struct.unpack('f', b_x3)
            b_x4 = data[12:16]
            f_x4 = struct.unpack('f', b_x4)
            b_road = data[16:20]
            f_road = struct.unpack('f', b_road)
            b_force = data[20:24]
            f_force = struct.unpack('f', b_force)

            print(float(f_road[0]), end='')
            print("   ", end='')
            print(float(f_force[0]))

class GUI(QtWidgets.QMainWindow):
    signal_start_udp_server = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super(GUI, self).__init__(parent)

        # Set box parameters
        self.setWindowTitle("Adaptive suspension symulator")
        self.setGeometry(100, 100, 600, 800)

        # Close gui
        button_close_gui = QtWidgets.QPushButton("Close")
        button_close_gui.clicked.connect(self.close)

        # Set plot paramaters
        self.plot_road = MplCanvas(self, width=5, height=4, dpi=100)
        self.plot_road.fig.suptitle("Road", fontsize=10)
        self.plot_control_force = MplCanvas(self, width=5, height=4, dpi=100)
        self.plot_control_force.fig.suptitle("Control signal force", fontsize=10)
        self.plot_model_state = MplCanvas(self, width=5, height=4, dpi=100)
        self.plot_model_state.fig.suptitle("Model state - Y", fontsize=10)
        self.update_plot()

        # Create toolbar, passing canvas as first parament, parent (self, the MainWindow) as second.
        toolbar_road = NavigationToolbar(self.plot_road, self)
        toolbar_control = NavigationToolbar(self.plot_control_force, self)
        toolbar_model = NavigationToolbar(self.plot_model_state, self)

        # create kpi push button
        kpi_button = QtWidgets.QPushButton("Calculate KPI from Y state");
        kpi_button.clicked.connect(self.calculate_kpi)

        # kpi label - value
        self.kpi_label = QLabel()
        self.calculate_kpi()

        # Setup box layout
        box_layout = QtWidgets.QVBoxLayout()
        box_layout.addWidget(button_close_gui)
        box_layout.addWidget(kpi_button)
        box_layout.addWidget(self.kpi_label)
        box_layout.addWidget(toolbar_road)
        box_layout.addWidget(self.plot_road)
        box_layout.addWidget(toolbar_control)
        box_layout.addWidget(self.plot_control_force)
        box_layout.addWidget(toolbar_model)
        box_layout.addWidget(self.plot_model_state)

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

        # Setup timers to start udp server
        self.timer_udp_server = QtCore.QTimer()
        self.timer_udp_server.setInterval(500)
        self.timer_udp_server.timeout.connect(self.start_udp_server)
        self.timer_udp_server.start()

    def update_plot(self):
        # plot road
        global road
        # clear plot
        self.plot_road.axes.cla()
        # set new plot axes values
        road_x = list(range(0, len(road)))
        self.plot_road.axes.plot(road_x, road)
        # Trigger the canvas to update and redraw.
        self.plot_road.draw()

        # plot control force
        global control_force
        # clear plot
        self.plot_control_force.axes.cla()
        # set new plot axes values
        control_force_x = list(range(0, len(control_force)))
        self.plot_control_force.axes.plot(control_force_x, control_force)
        # Trigger the canvas to update and redraw.
        self.plot_control_force.draw()

        # plot model states
        global model_states
        # clear plot
        self.plot_model_state.axes.cla()
        # set new plot axes values
        model_states_x = list(range(0, len(model_states)))
        self.plot_model_state.axes.plot(model_states_x, model_states)
        # Trigger the canvas to update and redraw.
        self.plot_model_state.draw()

    def calculate_kpi(self):
        new_kpi = "New kpi value model state Y: "  + str(randrange(10))
        self.kpi_label.setText(new_kpi)
        QApplication.processEvents()

    def start_udp_server(self):
        # start udp server
        self.signal_start_udp_server.emit()
        self.timer_udp_server.stop()

if __name__ == '__main__':

    app = QtWidgets.QApplication(sys.argv)
    gui = GUI()

    upd_server = UDPServer()
    thread_upd_server = QtCore.QThread()
    thread_upd_server.start()
    upd_server.moveToThread(thread_upd_server)

    gui.signal_start_udp_server.connect(upd_server.init_server)

    sys.exit(app.exec_())
