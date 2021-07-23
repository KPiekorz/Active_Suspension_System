#ifndef MODEL_SIMULATION_H
#define MODEL_SIMULATION_H

// #define INIT_MODEL_SIMULATION

typedef enum
{
    model_simulation_message_states,
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