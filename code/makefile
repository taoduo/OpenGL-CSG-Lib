CC=clang

LIBS=-lglfw -framework OpenGL

OBJ = main.o CSG.o CSGNode.o CSGPlane.o CSGTriangle.o CSGTriangleList.o CSGVertex.o CSGError.o CSGVertexList.o matrix.o primitives.o vector.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

csg: $(OBJ)
	clang -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ csg