#ifndef _COLINLI_PATH_H_
#define _COLINLI_PATH_H_ 


#include <string>

namespace colinli{
namespace utility{
namespace path{
/**
 * get the filename from the full path to the file 
 * @param  filepath the full path to the file, e.g. full/path/to/file.c 
 * @return          the file name, e.g. file.c from the above example 
 */
std::string GetFilenameFromFullPath(const std::string& filepath);

/**
* Check validity of filename, filename should not include illegal 
* chracters such as % / or // 
* @param  prefix_filename [filename]
* @return                 true: filename is valid false: invalid filename
*/
bool IsValidFilename(const std::string prefix_filename);
/**
* Fix up file full path by removing extraneous characters
* @param  path      base directory of file
* @param  file_name filename
* @return           fixed-up edition of full path to file 
*/
std::string PathSanityFix(std::string path, std::string file_name);



}// end of path ns 
}// end of utility ns
}// end of colinli ns
#endif
