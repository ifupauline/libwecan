# libwecan

                                  .::::::::.                      
                                  :::::::::::                     
                                  ':::::::::::..                  
                                   :::::::::::::::'               
          BECAUSE WE CAN!            ':::::::::::.                 
                                      .::::::::::::::'            
                                    .:::::::::::...               
                                   ::::::::::::::''               
                       .:::.       '::::::::''::::                
                     .::::::::.      ':::::'  '::::               
                    .::::':::::::.    :::::    '::::.             
                  .:::::' ':::::::::. :::::      ':::.            
                .:::::'     ':::::::::.:::::       '::.           
              .::::''         '::::::::::::::       '::.          
             .::''              '::::::::::::         :::...      
          ..::::                  ':::::::::'        .:' ''''     
       ..''''':'                    ':::::.'
                                                            
NICE TO HAVE
- Improve encode function by checking if signal fits in the given frame.
  Let's say we want to encode a signal on an 8 bytes frame that has a length of
  32 and a startbit of 48, assuming intel endianness(little) obviously signal
  will extend beyond boundary. 
  At the moment it is up to the developper to not fuck it up by making sure 
  startbit, length and endianness are coherent to the size of frame (dlc).
