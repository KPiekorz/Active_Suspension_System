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

#define GUI_MAX_FLOAT_DATA_LENGHT                       (40)

pthread_mutex_t mutex_udp_byte_data = PTHREAD_MUTEX_INITIALIZER;
#define GetUpdByteDataMutex()                           (&mutex_udp_byte_data)
#define UDP_DATA_BYTE_MAX_SIZE                          (100)
byte upd_byte_data[UDP_DATA_BYTE_MAX_SIZE];
int udp_byte_data_size;

#define PYTHON_GUI_ADDRESS      (INADDR_ANY)
#define INVALID_SOCKET          (-1)

const char *gui_fifo_name = "gui_fifo";

/*** STATIC FUNTION ***/

static bool gui_UdpClientCreate(int * my_socket)
{
	int s = socket(PF_INET, SOCK_DGRAM, 0);

	if (s == -1)
    {
        DEBUG_LOG_ERROR("[GUI] gui_UdpClientCreate, Cannot create socket.");
		return false;
	}

    *my_socket = s;

    return true;
}

static void gui_UdpClientDestroy(int my_socket)
{
	/* Clean up */
    close(my_socket);
}

static bool gui_UdpClientSendData(int my_socket, byte * data, int data_len)
{
	/* Socket address structure */
	struct sockaddr_in socket_addr;

	/* Initialize socket address to 0*/
	memset(&socket_addr, 0, sizeof(socket_addr));
	/* Set socket address parameters */
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(1100);
	socket_addr.sin_addr.s_addr = PYTHON_GUI_ADDRESS;

	/* Send message to server */
    if (sendto(my_socket, data, data_len, MSG_CONFIRM, (const struct sockaddr *) &socket_addr, sizeof(socket_addr)) <= 0)
    {
        return false;
    }

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
        /* delay for waiting for another message */
        DELAY_MS(RECEIVE_FIFO_MESSAGE_SYSTEM_DELAY_MS);

        /* try receive message */
        float_data_len = GUI_MAX_FLOAT_DATA_LENGHT;
        if (true == SystemUtility_ReceiveMessage(gui_fifo_name, (int *)&message_type, float_data, &float_data_len))
        {
            switch (message_type)
            {
                case gui_message_model_simulation_data:
                    pthread_mutex_lock(GetUpdByteDataMutex());

                    /* set float data in byte array */
                    udp_byte_data_size = SystemUtility_SetFloatArrayInByteArray(float_data, float_data_len, upd_byte_data, UDP_DATA_BYTE_MAX_SIZE);

                    pthread_mutex_unlock(GetUpdByteDataMutex());
                break;
                default:
                    DEBUG_LOG_WARN("[GUI] gui_ReceiveMessageThread, unknown message type!");
                break;
            }

            // for (int i = 0; i < float_data_len; i++)
            // {
            //     DEBUG_LOG_VERBOSE("[GUI] gui_ReceiveMessageThread, float[%d]: %f", i, float_data[i]);
            // }
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
        pthread_mutex_lock(GetUpdByteDataMutex());

        if (udp_byte_data_size > 0)
        {
            if (false == gui_UdpClientSendData(my_socket, upd_byte_data, udp_byte_data_size))
            {
                DEBUG_LOG_ERROR("[GUI] gui_UdpClientThread, can't send udp data, data len: %d, socket id: %d", data_len, my_socket);
            }
        }

        /* clean send udp data */
        udp_byte_data_size = 0;
        memset(upd_byte_data, 0, UDP_DATA_BYTE_MAX_SIZE);

        pthread_mutex_unlock(GetUpdByteDataMutex());

        DELAY_MS(10);
    }

    gui_UdpClientDestroy(my_socket);

    return 0;
}

/*** GLOBAL FUNCTION ***/

void Gui_Init(void)
{
    #ifdef  INIT_GUI

        /* Init pipe connection to gui process */
        if (!SystemUtility_CreateThread(gui_ReceiveMessageThread))
        {
            DEBUG_LOG_ERROR("[GUI] Gui_Init, Can't create receive gui thread!");
        }

        /* Init udp connection to python gui */
        if (!SystemUtility_CreateThread(gui_UdpClientThread))
        {
            DEBUG_LOG_ERROR("[GUI] Gui_Init, Can't create udp client thread!");
        }

        #ifdef INCLUDE_PYTHON_GUI
            /* Start gui python app */
            gui_RunGui();
        #endif /* INCLUDE_PYTHON_GUI */

        /* event loop for gui process */
        while (1)
        {
            DELAY_S(SYSTEM_EVENT_LOOP_DELAY_S);
        }

        exit(EXIT_SUCCESS);
    #else
        DEBUG_LOG_INFO("[GUI] Gui_Init, Won't be initialized.");
    #endif
}

void Gui_Destroy(void)
{

}

void Gui_SendMessage(gui_message_type_t message_type, float * data, int data_len)
{
    // this message will be received in gui_ReceiveMessageThread
    if (!SystemUtility_SendMessage(gui_fifo_name, (int)message_type, data, data_len))
    {
        DEBUG_LOG_ERROR("[GUI] Gui_SendMessage, Can't send message to gui process!");
    }
}