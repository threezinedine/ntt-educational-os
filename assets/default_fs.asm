dd 0x33344333
dd 0x00000200 ; 512 node blocks
dd 0x00000100 ; 256 bytes per node block; 1 sector has 2 blocks
dd 0x00000001 ; 1 byte per inode meta data

; so that has totally 1 table sector

times 512-($-$$) db 0
db 0x01 ; home directory

times 512 * 2 -($-$$) db 0
db 'root', 0
times 512 * 2 + 64 - ($ - $$) db 0
db 0x01         ; is directory
db 0x00         ; has no next inode
dw 0x0000       ;