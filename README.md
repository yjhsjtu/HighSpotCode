# HighSpotCode
1. The source code is compiled under Microsoft Visual Studio Enterprise 2019 Version 16.4.2 and execute under windows
2. Build the code and run under the release directory from command line as below
         HighSpotCode\Release> HighSpotCode.exe mixtape-data.json change.json output.json
3. The json parser load whole file mixtape-data.json and change.json into memory and parse them. 
   If these files are too big to be loaded in memory, the program can not work
4. To make the program work for big files that can not be loaded in memory. We can use the following algorithm:
  a. sort mixtape-data.json by playlist id in ascending order using merge sort (details in step 5).
  b. sort change.json by playlist id in ascending order using merge sort
  c. load a small chunk of playlists from mixtape-data.json and change.json seperately into memory. 
  d. Compare the playlist id from two chunks.
      If they are same
        For remove playlist change, skip it and move to next playlist in both chunks
        For update playlist change, update playlist in both chunk and output the merged playlist to output.json. move to next playlist in         both chunks
        For add new playlist change, throw error message and exit since newly added playlist should has new id
      if mixtape-data.json chunk has smaller id, move to next playlist
      if change.json chunk has smaller id, output the playlist to output.json and move to next playlist
      When a chunk is ended, read next chunk in the file
 5. how to sort big file by playlist in ascending order. Assume memory is 4G and file is 100G
    a. split file into 2G chunks. Load each chunk in memory and quick sort in memory, output the sorted result to a file
    b. Split each 2G chunk of the sorted file into 0.4G size blocks. load a block from each chunk in memory, totally we have 50*0.4=2G.
    c. Merge sort playlists from 50 blocks. Get one playlist from each block and put them in a min priority queue with size 50. 
    Get the playlist with minimal id and write it to the output file. Fetch the nxet playlist in the same block and put it in the queue.
    After a whole block is processed, read the next block from the same chunk into memory.
  
