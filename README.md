# HighSpotCode
1. Compile the code using Microsoft Visual Studio Enterprise 2019 Version 16.4.2 under windows
2. Build the code and run under the release directory from command line as below
         HighSpotCode\Release> HighSpotCode.exe mixtape-data.json change.json output.json
3. The json parser I used is nlohmann json. It loads whole file mixtape-data.json and change.json into memory and parse them. If these files are too big to be loaded into memory, the program can not work as expected
4. To make the program work for big files that can not be loaded in memory, We can take the following steps:
    * sort mixtape-data.json by playlist id in ascending order using merge sort (details in step 5).
    * sort change.json by playlist id in ascending order using merge sort
    * load a small chunk of playlists from mixtape-data.json and change.json seperately into memory. 
    * compare the playlist idS from two chunks. If they are same, For remove playlist change, skip it and move to next playlist in both chunks. For update playlist change, merge playlist details from both chunk and output the merged playlist to output.json. Move to the next playlist in both chunks. For add new playlist change, throw error message and exit
    * if mixtape-data.json chunk has smaller id, move to its next playlist
    * if change.json chunk has smaller id, output the playlist to output.json and move to its next playlist.
    * when playlist from a chunk is processed, read next chunk from the same file
 5. To sort big file by playlist in ascending order, we can take folloing steps. To help explain the steps, we assume memory is 4G and file is 100G and 2G is allowed to be used in memory.
    * split file into 2G chunks. Load each chunk in memory and quick sort in memory, output the sorted result to a file
    * Split each 2G chunk of the sorted file into 0.4G size blocks. load a block from each chunk in memory, totally we have 50*0.4=2G.
    * Merge sort playlists from 50 blocks. Get one playlist from each block and put them in a min priority queue with size 50. 
    Get the playlist with minimal id and write it to the output file. Fetch the nxet playlist in the same block and put it in the queue.
    After a whole block is processed, read the next block from the same chunk into memory.
 
  
