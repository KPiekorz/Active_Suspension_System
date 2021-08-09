#ifndef MODEL_SIMULATION_H
#define MODEL_SIMULATION_H

/*** INCLUDE MODEL SIMULATION PROCESS ***/
#define INIT_MODEL_SIMULATION

/*** DEFINE MODEL SIMULATION MESSAGE DATA ***/
#define MODEL_SIMULATION_STATE_X1           (0)
#define MODEL_SIMULATION_STATE_X1_DOT       (1)
#define MODEL_SIMULATION_STATE_Y1           (2)
#define MODEL_SIMULATION_STATE_Y1_DOT       (3)
#define MODEL_SIMULATION_STATE_Y2           (4) // int(X1-X2) = int(Y1)
#define MODEL_SIMULATION_STATE_ROAD         (5)
#define MODEL_SIMULATION_STATE_FORCE        (6)
#define MODEL_SIMULATION_STATE_ITERATION    (7)
#define MODEL_SIMULATION_DATA_SIZE          (8)


/*** SIMULATION PARAMETERS ***/

#define SIM_TIME       (60)
#define SAMPLE_TIME    (0.01f)

typedef enum
{
    simulation_message_unknown = 0,
    simulation_message_control_force,
} model_simulation_message_type_t;

/**
 * @brief  Model simulation init process
 * @note
 * @retval None
 */
void ModelSimulation_Init(void);

/**
 * @brief  Model simulation destroy process
 * @note
 * @retval None
 */
void ModelSimulation_Destroy(void);

/**
 * @brief  Sand message to gui process, form any other procerss from the system (pipe).
 *         It will give other process to communicate with each other win convenient way.
 *         (it is only used to communicate with oher process among system, not with other
 *          threads in one process)
 * @note
 * @param  data: pointer to data to send (best will be array)
 * @param  data_len: data lenght in bytes
 * @retval None
 */
void ModelSimulation_SendMessage(model_simulation_message_type_t message_type, float * data, int data_len);

#endif /* MODEL_SIMULATION_H */