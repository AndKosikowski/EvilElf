using Gtk
filename = open_dialog("Choose a file you dimwit")

#https://wiki.osdev.org/ELF

#https://stackoverflow.com/questions/12822570/how-do-i-check-if-an-object-file-is-in-coff-or-elf-format-from-within-c#:~:text=Read%20the%20first%20four%20bytes,see%20if%20it%20makes%20sense.
#ELF magic number is 0x7f454C46 (0x7f + "ELF")

#all of this stuff may only work on certain endianness currently

#objdump also valid

function endianAdjustedRead(f,amount,endianness)
    result = []
    if endianness == 1
        result = read(f,amount)
    elseif endianness == 2
        result = reverse(read(f,amount))
    end
    result
end

open(filename) do f
    fileType = read(f,4)

    isElf = Array{UInt8,1}(undef, 4)
    isElf[1] = 0x7f
    isElf[2] = 0x45
    isElf[3] = 0x4C
    isElf[4] = 0x46
    if(fileType == isElf)
        print("ELF file\n")
    else
        print("Not ELF (Or maybe wrong endianness?)")
        return
    end
    
    
    numBits = read(f,1)[1]
    if(numBits == 1)
        print("32 bit\n")
    elseif (numBits == 2)
        print("64 bit\n")
    else 
        print("Unknown bit\n")
        return
    end

    endianness = read(f,1)[1]
    if(endianness == 1)
        print("Little Endian\n")
    elseif (endianness == 2)
        print("Big Endian\n")
    else 
        print("Unknown Endianness\n")
        return
    end

    #https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html

    elfHeaderVersion = endianAdjustedRead(f,1,endianness)
    print("ELF Header Version: " * string(elfHeaderVersion[1]) * "\n")
    
    osABI = endianAdjustedRead(f,1,endianness)
    print("OS ABI: " * string(osABI[1]) * "\n")

    padding = endianAdjustedRead(f,8,endianness)

    
    


end



