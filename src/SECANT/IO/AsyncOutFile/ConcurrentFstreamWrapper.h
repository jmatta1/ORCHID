/**
 * @brief Waits for lock of the file, closes the file and opens the new one
 * @param[in] fileNumber the index of the file whose name is to be changed
 * @param[in] filePath a constant reference to the string that contains the path to be written
 * 
 * This function will block the calling thread until the thread can lock the
 * file to change the file name etc. If multiple threads are writing to the
 * same file, you may not like the result if they all try to use this
 * function simultaneously... maybe
 */
void changeFileName(int fileNumber, const std::string& filePath);
