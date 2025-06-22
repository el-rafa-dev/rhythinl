#!/usr/bin/python
import os

def repl():
    """ It is necessary to control the number of times the code was executed. the maximum accepted is 1x """
    exec_times = int(0) #default value
    ## the cmakecache path
    cmake_path = os.getcwd() + "/CMakeCache.txt"

    with open(cmake_path, "r", encoding="utf-8") as f:
        txt = "# Generated with gencmake.py\n" + f.read()
        txto = txt.replace("/home/el-rafa/VSCode/Rhythin", os.getcwd())
        
    if (exec_times < 1):
        with open(cmake_path, "w", encoding="utf-8") as f:
            if (f.write(txto)):
                exec_times += 1
                print(f"ExecTime: {exec_times}")
                print(f"CMakeCache generated with sucess to path {cmake_path}!")            
            else:
                exec_times = 0
                print(f"ExecTime: {exec_times}")
                print("Cannot generate the CMakeCache file")
    else:
        print(f"ExecTime: {exec_times}")
        print("gencmake.py only accepts 1 execution times ")

    

if __name__ == "__main__":
    repl()
