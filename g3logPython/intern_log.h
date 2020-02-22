
namespace g3 {
    
void receivelog(const char *file, int line, const char* functionname, int level_val, const char *message);

enum class pyLEVEL : int
{
    pyDEBUG,
    pyINFO,
    pyWARNING,
    pyFATAL
};

} // g3
