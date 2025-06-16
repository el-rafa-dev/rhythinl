#!/usr/bin/python
import os

def repl():
    """ It is necessary to control the number of times the code was executed. the maximum accepted is 1x """
    exec_times:int = 0 #default value
    ## the cmakecache path
    cmake_path = os.getcwd() + "/CMakeCache.txt"

    with open(cmake_path, "r", encoding="utf-8") as f:
        txt = "\n#Generated with gencmake.py\n" + f.read()
        txto = txt.replace("/home/el-rafa/VSCode/Rhythin", os.getcwd())
        
    if (exec_times < 1):
        with open(cmake_path, "w", encoding="utf-8") as f:
            if (f.write(txto)):
                print(f"CMakeCache generated with sucess to path {cmake_path}!")            
                #increase 1 to executed times variable
                exec_times = 1
            else:
                print("Cannot generate the CMakeCache file")
    elif (exec_times == 1):
        print("gencmake.py only accepts 1 execution times ")

    

if __name__ == "__main__":
    repl()