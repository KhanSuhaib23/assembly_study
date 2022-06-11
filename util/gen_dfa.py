character_classes = [
    ('Null', 'nl', '0x00'),
    ('Letter', 'lt', '0x01'),
    ('Digit', 'di', '0x02'),
    ('Invalid', 'iv', '0x03'),
]

states = [
    ('Ident_Complete', 'ic'),
    ('Got_Letter', 'gl'),
    ('Start', 'st'),
]

character_class_map = [
    ('~', 'iv'),
    ('0', 'nl'),
    ('65-90', 'lt'),
    ('97-122', 'lt'),
    ('48-57', 'di'),
]

state_transitions = [
    ('st', '~', 'st'),
    ('st', 'lt', 'gl'),
    ('gl', '~', 'ic'),
    ('gl', 'lt,di', 'gl'),
    ('ic', '~', 'st'),
    ('ic', 'lt', 'gl'),
]


def write_enum(enum_defs, name, mult = None):
    enum_str = "typedef enum {\n"

    for i, cc in enumerate(enum_defs):
        if len(cc) == 3:
            enum_str += '    {}_{} = {},\n'.format(name, cc[0], cc[2])
        elif len(cc) == 2:
            if mult == None:
                enum_str += '    {}_{},\n'.format(name, cc[0])
            else:
                enum_str += '    {}_{} = {},\n'.format(name, cc[0], i * mult)


    enum_str += '}} {};\n'.format(name)

    print(enum_str)

def write_enum_define(enum_defs, name):
    oc_enum = ''

    for cc in enum_defs:
        oc_enum += '#define {} {}_{}\n'.format(cc[1], name, cc[0])

    print(oc_enum)


def write_enum_undef(enum_defs):
    oc_enum = ''

    for cc in enum_defs:
        oc_enum += '#undef {}\n'.format(cc[1])

    print(oc_enum)

def get_size_in_bits(num):
    bits = 0
    while num > 0:
        bits += 1
        num //= 2

    return bits

def get_bits_per_state(num_states):
    for i in range(2, 17):
        if get_size_in_bits(i * (num_states - 1)) <= i:
            return i

    return None

def get_table_entry_size(num_states, bits_per_state):
    for b in [8, 16, 32, 64]:
        if bits_per_state * num_states <= b:
            return b
    return None

def get_table_config(num_states):
    bps = get_bits_per_state(num_states)
    b = None if bps == None else get_table_entry_size(num_states, bps)

    if b == None:
        bps = get_size_in_bits(num_states)
        b = get_table_entry_size(num_states, bps)

    if b == None:
        bps = get_size_in_bits(num_states)
        for dt in [8, 16, 32, 64]:
            if bps <= dt:
                return (None, dt)

        return (None, None)

    return (b, bps)

def get_name_num(map_list, state_name):
    for i, state in enumerate(map_list):
        if state[0] == state_name or state[1] == state_name:
            return i
    return None


label = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']

output_map = [None] * 256
state_map = [None] * (len(character_classes) * len(states))

for item in character_class_map:
    k = item[0]
    v = item[1]

    if k == '~':
        for i in range(0, 256):
            output_map[i] = v
        continue

    elements = k.split(',')

    for element in elements:
        r = element.split('-')
        if len(r) == 1:
            output_map[int(r[0])] = v
        elif len(r) == 2:
            l = int(r[0])
            h = int(r[1])

            for i in range(l, h + 1):
                output_map[i] = v


for item in state_transitions:
    s = get_name_num(states, item[0])
    t = None

    if item[1] == '~':
        t = range(0, len(character_classes))
    else:
        t = []
        for x in item[1].split(','):
            t.append(get_name_num(character_classes, x))

    for cc in t:
        state_map[s + cc * len(states)] = item[2]


num = len(states)

table_entry_sz, bps = get_table_config(num)
assert (table_entry_sz != None or bps != None)

state_mult = 0
table_type = ''
lut_define = '#define lut('

for i, _ in enumerate(states):
    if i == len(states) - 1:
        lut_define += 's{}'.format(i)
    else:
        lut_define += 's{}, '.format(i)

lut_define += ') \\\n'


if table_entry_sz == 0:
    state_mult = len(character_classes)
    for i, x in enumerate(states):
        lut_define += 's{}, '.format(i)

    table_type = 'uint{}_t'.format(bps)
else:
    state_mult = bps
    lut_define = '#define BITS_PER_STATE {}\n\n'.format(bps) + lut_define

    lut_define += '    (uint{}_t) ('.format(table_entry_sz)

    for i, x in enumerate(states):
        if i > 0:
            lut_define += ' |\\\n'
            lut_define += '                '

        lut_define += '((s{}) << (BITS_PER_STATE * {}))'.format(i, i)

    lut_define += ')'

    table_type = 'uint{}_t'.format(table_entry_sz)
    

write_enum(character_classes, 'Character_Class')
write_enum_define(character_classes, 'Character_Class')

write_enum(states, 'State', state_mult)
write_enum_define(states, 'State')

st_fmt = '/* {} */    '

index_width = max(map(lambda x: len(x), label))
width = max(map(lambda x : len(x), output_map)) + 2
st_header = '//'.ljust(len(st_fmt) - 2 + index_width) + ''.join(map(lambda x : x.rjust(width) + ' ', label))

print('const uint8_t character_class[] = {')
print(st_header)

for i in range(0, 16):
    row = st_fmt.format(label[i])
    for j in range(0, 16):
        row += output_map[i * 16 + j].rjust(width) + ','
    print(row)
print('};\n\n')


print('{}\n'.format(lut_define))
st_fmt = '/* {} */    lut('

index_width = max(map(lambda x: len(x[1]), character_classes))
width = max(map(lambda x : len(x), state_map)) + 2
st_header = '//'.ljust(len(st_fmt) - 2 + index_width) + ''.join(map(lambda x : x[1].rjust(width) + ' ', states))

print('const {} dfa[] = {{'.format(table_type))
print(st_header)

for i, cc in enumerate(character_classes):
    row = st_fmt.format(cc[1])
    for j, st in enumerate(states):
        row += state_map[i * len(states) + j].rjust(width)
        if j < len(states) - 1:
            row += ','
    row += ' ),'
    print(row)
print('};\n\n')

print('#undef lut\n')

write_enum_undef(character_classes)
write_enum_undef(states)
