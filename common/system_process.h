#ifndef SYSTEM_PROCESS_H
#define SYSTEM_PROCESS_H

/**
 * @brief  Init all system processes and save every process id
 * @note   
 * @retval None
 */
void SystemProcess_Initialize(void);

/**
 * @brief  Kill all known system process (when proper sig will be invocked) 
 * @note   
 * @retval None
 */
void SystemProcess_Destroy(void);

/**
 * @brief  Print all process pid
 * @note   
 * @retval None
 */
void SystemProcess_PrintProcessPid(void);

/**
 * @brief  Print system process pid
 * @note   
 * @retval None
 */
void SystemProcess_PrintAllProcessPid(void);

#endif  /* SYSTEM_PROCESS_H */