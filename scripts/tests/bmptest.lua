vprint("Tests bitmap loading\n")

z = loadBitmap("images\\knob.bmp")
size = getBitmapSize(z)
vprint("Width = "..size.width.." Height="..size.height)
