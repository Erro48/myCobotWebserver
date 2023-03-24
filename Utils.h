#ifndef __UTILS__
#define __UTILS__

/**
 * Class with some generic functions.
 */
class Utils {
public:
  /**
   * Reads the file and returns the content.
   * @param fileName the name of the file to read.
   * @returns the file content.
   */
  static char* readFile(const char* fileName);

  /**
   * Write the content into the file.
   * @param fileName the name of the file to read.
   * @param content the content to write.
   * @returns `true` if it writes correctly, `false` otherwise.
   */  
  static bool writeFile(const char* fileName, const char* content);

};

#endif