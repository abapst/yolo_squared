#!/usr/bin/python
import sys,os

"""
Get a list of all files in a directory with a given extension
"""
def get_filenames(inpath,ext):
    file_list = []
    lst = os.listdir(inpath)
    lst.sort()
    for filename in lst:
        if filename.endswith(ext):
            file_list.append(filename.split(".")[0])
    return file_list 

"""
Write a list of files to an output file.
The list format is:
	Line 1: number of files to read
	Line 2: path to the files
	Line 3: file extension
	Line 4 -> Line 4 + nFiles: filenames without their extensions
"""
def write_filenames(file_list,outname,inpath,ext):
    outfile = open(outname,"w")
    outfile.write("{0}\n".format(len(file_list))) # write number of videos
    outfile.write("{0}\n".format(inpath))         # write filepath
    outfile.write("{0}\n".format(ext))            # write extension
    for filename in file_list:
        outfile.write("{0}\n".format(filename))
    outfile.close()

if __name__ == "__main__":
    """
    usage:
        python build_filelist.py <input directory> <output filename> <ext>
    """
    inpath = sys.argv[1]
    outname = sys.argv[2]
    ext = sys.argv[3]
    file_list = get_filenames(inpath,ext)
    write_filenames(file_list,outname,inpath,ext)
