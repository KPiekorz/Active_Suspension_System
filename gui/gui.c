#include "gui.h"
#include <python3.7/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "system_utility.h"
#include "model_simulation.h"

#define INCLUDE_PYTHON_GUI

#define PYTHON_GUI_ADDRESS      (INADDR_ANY)
#define INVALID_SOCKET          (-1)

const char *gui_fifo_name = "gui_fifo";

/*** STATIC FUNTION ***/

static bool gui_UdpClientCreate(int * my_socket)
{
    DEBUG_LOG_DEBUG("[GUI] Init udp client.");

	int s = socket(PF_INET, SOCK_DGRAM, 0);

	if (s == -1)
    {
        DEBUG_LOG_ERROR("[GUI] Cannot create socket.");
		return false;
	}

    *my_socket = s;

    return true;
}

static void gui_UdpClientDestroy(int my_socket)
{
    DEBUG_LOG_DEBUG("[GUI] Init udp client.");

	/* Clean up */
    close(my_socket);
}

static bool gui_UdpClientSendData(int my_socket, byte * data, int data_len)
{
    DEBUG_LOG_DEBUG("[GUI] gui_UdpClientSendData, socket num: %d, data len: %d", my_socket, data_len);

	/* Socket address structure */
	struct sockaddr_in socket_addr;

	/* Initialize socket address to 0*/
	memset(&socket_addr, 0, sizeof(socket_addr));
	/* Set socket address parameters */
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(1100);
	socket_addr.sin_addr.s_addr = PYTHON_GUI_ADDRESS;

	/* Send a message to server */
    sendto(my_socket,
          data,
          data_len,
          MSG_CONFIRM,
          (const struct sockaddr *) &socket_addr,
          sizeof(socket_addr));

    return true;
}

static void gui_RunGui(void)
{

#ifdef INCLUDE_PYTHON_GUI

    Py_Initialize();

    errno = 0;
    system("chmod +x ./gui/gui_plot.py");
    FILE *PScriptFile = fopen("./gui/gui_plot.py", "r");

    if (PScriptFile == NULL)
    {
        DEBUG_LOG_ERROR("File to open py gui, Error: %d", errno);
    }
    else
    {
        PyRun_SimpleFile(PScriptFile, "gui_plot.py");
        fclose(PScriptFile);
    }

    Py_Finalize();

#endif /* INCLUDE_PYTHON_GUI */
}

static void * gui_ReceiveMessageThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    /* create message fifo queue */
    if (SystemUtility_CreateMessageFifo(gui_fifo_name) == false)
    {
        DEBUG_LOG_ERROR("[GUI] gui_ReceiveMessageThread, Can't create FIFO!");
        return 0;
    }

    gui_message_type_t message_type = gui_message_unknown;
    int float_data_len = 20;
    float float_data[float_data_len];

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, Wait for message!");

        /* delay for waiting for another message */
        DELAY_MS(10);

        /* try receive message */
        if (true == SystemUtility_ReceiveMessage(gui_fifo_name, (int *)&message_type, float_data, &float_data_len))
        {
            DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, message type: %d, float data len: %d", message_type, float_data_len);

            for (int i = 0; i < float_data_len; i++)
            {
                DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
            }
        }
    }

    return 0;
}

static void * gui_UdpClientThread(void *cookie)
{
    /* init thread with good priority */
    SystemUtility_InitThread(pthread_self());

    int my_socket = INVALID_SOCKET;
    if (false == gui_UdpClientCreate(&my_socket))
    {
        return 0;
    }

    int data_len = 3;
    byte data[3] = {1, 2, 3};

    while (true)
    {
        DEBUG_LOG_VERBOSE("[GUI] gui_UdpClientThread");

        if (true == gui_UdpClientSendData(my_socket, data, data_len))
        {

        }

        DELAY_S(2);
    }

    gui_UdpClientDestroy(my_socket);

    return 0;
}

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    #ifdef  INIT_GUI
        DEBUG_LOG_INFO("[GUI] Gui_Init, Init process...");

        /* Init pipe connection to gui process */
        if (SystemUtility_CreateThread(gui_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[GUI] Gui_Init, Can't create receive gui thread!");
        }

        /* Init udp connection to python gui */
        if (SystemUtility_CreateThread(gui_UdpClientThread))
        {
            DEBUG_LOG_ERROR("[GUI] Gui_Init, Can't create udp client thread!");
        }

        /* Init udp socket connection to python gui app */
        // gui_InitUdpSocketConnectionToPythonPlot();

        /* Start gui python app */
        // gui_RunGui();

        while (1)
        {
            DEBUG_LOG_VERBOSE("[GUI] Gui_Init, process running... (should never enter here).");
            DELAY_S(5);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[GUI] Gui_Init, Won't be initialized.");
    #endif
}

void Gui_Destroy(void)
{
    DEBUG_LOG_DEBUG("[GUI] Destroy process...");
}

void Gui_SendMessage(gui_message_type_t message_type, float * data, int data_len)
{
    DEBUG_LOG_DEBUG("[GUI] Gui_SendMessage, message type: %d, len: %d", message_type, data_len);

    // this message will be received in gui_ReceiveMessageThread
    if (!SystemUtility_SendMessage(gui_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[GUI] Gui_SendMessage, Can't send message to gui process!");
    }
}