dd 0x33344333
dd 0x00000200 ; 512 node blocks
dd 0x00000100 ; 512 bytes per node block; 1 sector has 1 block
dd 0x00000001 ; 1 byte per inode meta data

; so that has totally 1 table sector

times 512-($-$$) db 0
db 0x01 ; home directory
db 0x01 ; test directory
db 0x01 ; hello.txt

times 512 * 2 -($-$$) db 0
db 'root', 0
times 512 * 2 + 64 - ($ - $$) db 0
db 0x01         ; is directory
db 0x00         ; has no next inode
dw 0x0000       ;
dw 0x0001           ; test directory
dw 0x0002           ; hello.txt file

times 512 * 3 - ($ - $$) db 0

db 'test', 0
times 512 * 3 + 64 - ($ - $$) db 0
db 0x01         ; is directory
db 0x00         ; has no next inode
dw 0x0000       ;

times 512 * 4 - ($ - $$) db 0
db 'hello.txt', 0
times 512 * 4 + 64 - ($ - $$) db 0
db 0x00         ; is a file 
db 0x00         ; has no next inode
dw 0x0000       ; 