import sys;

# set this parametre before running
model_name = "input"

if len(sys.argv) > 1:
    model_name = sys.argv[1][:-4]

# first read to identify faces count
f_in = open(model_name+".msh", 'r')
for line in f_in:
    if line == "$Elements\n":
        break
empty_var, faces_count, min_f_in, max_f_in = map(int, next(f_in).split(' '))
f_in.close()

# second read to rewrite in OFF format
f_in = open(model_name+".msh", 'r')
vert_order = {}

for line in f_in:
    if line == "$Nodes\n":
        break

empty_var, nodes_count, min_in, max_in = map(int, next(f_in).split(' '))
next(f_in)   # skipping unnecessary info

for i in range(0, nodes_count):
    vert_order[int(next(f_in))] = i

# writing head info
f_out = open(model_name + ".off", 'w')
f_out.write("OFF\n")
f_out.write(str(nodes_count) + ' ' + str(faces_count) + " 0\n")

# writing nodes coordinates
for line in f_in:
    if line != "$EndNodes\n":
        f_out.write(line)
    else:
        break;

# skipping unnecessary information
next(f_in)
next(f_in)
next(f_in)

# writing polyhedrals
for line in f_in:
    if line != "$EndElements\n":
        inds = list(map(int, line.split(' ')[1:-1]))
        f_out.write("3  " + str(vert_order[inds[0]]) + ' ' +
                            str(vert_order[inds[1]]) + ' ' +
                            str(vert_order[inds[2]]) + '\n')
    else:
        break

f_out.close()
f_in.close()
