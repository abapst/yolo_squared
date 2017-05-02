import os
import sys

"""
This script modifies the network input dims in the cfg file for a
YOLO model
"""

def set_net_dims(model,dim):
    model_cfg = "models/" + model + "/" + model + ".cfg"
    temp_cfg = "temp.cfg"

    with open(temp_cfg,"wt") as fout:
        with open(model_cfg,"rt") as fin:
            for line in fin:
                if "width=" in line:
                    fout.write("width=" + str(dim) + "\n")
                elif "height=" in line:
                    fout.write("height=" + str(dim) + "\n")
                else:
                    fout.write(line)
    fout.close()
    fin.close()
    os.rename(temp_cfg,model_cfg)

if __name__ == "__main__":
    model = sys.argv[1]
    dim = int(sys.argv[2])
    set_net_dims(model,dim)
     
