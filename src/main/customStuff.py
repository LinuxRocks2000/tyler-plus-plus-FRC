print("Adding custom libraries")
import os, sys, shutil


includes = ["/home/awesome/desk/programs/python-programs/tyler++/tyler++better.hpp", "/home/awesome/desk/programs/python-programs/tyler++/StringStream.hpp"]

if os.path.isdir("src/main/include/custom"):
    shutil.rmtree("src/main/include/custom")
os.mkdir("src/main/include/custom")


for include in includes:
    print("Copying", include, "to src/main/include/custom")
    shutil.copy(include, "src/main/include/custom")
