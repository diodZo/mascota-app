"""Explicit provisional forms, not original/final art. Standard library only."""
from pathlib import Path
import importlib.util,re
ROOT=Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('baby_placeholder',Path(__file__).with_name('generate_placeholder.py'))
base=importlib.util.module_from_spec(spec);spec.loader.exec_module(base)
# Stable IDs are read from the central C++ catalogue, not maintained twice.
SPECIES=re.findall(r'\{Species::\w+,\s*"([a-z_]+)"', (ROOT/'core/evolution/catalog.hpp').read_text(encoding='utf-8'))
if not SPECIES:raise RuntimeError('No species found in central catalogue')
CLIPS={'idle':(16,4,True),'happy':(6,8,False),'walk':(4,8,True),'sad':(3,2,True),'eat':(4,6,False),'sleep':(2,1,True)}
def image(species,stage,kind,index):
 p=bytearray(64*64*4)
 def box(x,y,w,h,c):
  for yy in range(max(y,0),min(y+h,64)):
   for xx in range(max(x,0),min(x+w,64)):p[(yy*64+xx)*4:(yy*64+xx+1)*4]=bytes(c)
 def ellipse(x,y,w,h,c):
  for yy in range(y,y+h):
   for xx in range(x,x+w):
    if ((xx-x-w/2)/(w/2))**2+((yy-y-h/2)/(h/2))**2<=1:box(xx,yy,1,1,c)
 ink=(88,65,91,255);pink=(242,161,173,255);cream=(255,239,188,255)
 bodies=[(255,224,151,255),(238,177,164,255),(112,197,216,255),(188,172,225,255),(126,198,174,255)]
 body=bodies[SPECIES.index(species)%len(bodies)];head=2 if kind=='sad' else -1 if kind=='idle' and index%8==5 else index%2 if kind=='sleep' else 0
 feet=((0,-2),(-2,0),(0,2),(2,0))[index%4] if kind=='walk' else (0,0)
 if species=='pato_conejo':p=base.frame('happy' if kind=='eat' else kind,index%CLIPS['happy' if kind=='eat' else kind][0])
 else:
  if species=='gata_gallina':
   for i in range(12):box(13+i//2,14+i+head,2*(12-i//2),1,ink);box(35+i//2,14+i+head,2*(12-i//2),1,ink)
   ellipse(10,25+head,44,29,ink);ellipse(12,27+head,40,25,body)
   box(28,15+head,5,11,pink);box(34,18+head,5,8,pink)
   box(6,39,9,9,ink);box(8,40,5,6,cream);box(49,39,9,9,ink);box(51,40,5,6,cream)
  elif species=='pez_caminante':
   for i in range(16):box(4+i//2,24+i+head,9-i//2,1,ink)
   ellipse(12,22+head,43,31,ink);ellipse(14,24+head,39,27,body)
   box(25,17+head,13,5,ink);box(29,18+head,7,3,pink)
   box(8,42,8,3,ink);box(51,42,8,3,ink)
  elif species=='poni':
   box(16,37,34,16,ink);box(18,39,30,12,body)
   box(13,17+head,12,9,ink);box(11,25+head,29,23,ink);box(13,27+head,25,19,body)
   box(7,35+head,21,10,ink);box(9,37+head,17,6,body)
   box(30,24+head,11,17,pink);box(44,39,12,5,pink);box(50,43,7,8,pink)
  else:
   box(12,22+head,40,29,ink);box(14,24+head,36,25,body)
   box(17,29+head,30,14,ink);box(29,14+head,3,8,ink);box(27,12+head,7,4,pink)
   box(5,36,7,13,ink);box(52,36,7,13,ink);box(24,50,16,5,ink)
  box(17,53+feet[0],11,6,ink);box(19,54+feet[0],7,3,body);box(37,53+feet[1],11,6,ink);box(39,54+feet[1],7,3,body)
  eyes=cream if species=='chatgpt_junior' else ink
  for x in (21,39):
   if kind in ('sleep','happy'):box(x,35+head,5,2,eyes)
   elif kind=='idle' and index==14:box(x,36+head,5,1,eyes)
   else:box(x,33+head,4,5,eyes)
   if kind=='sad':box(x-1,31+head,4,1,ink)
  box(29,43+head,6,2,ink)
  if kind=='happy':box(30,45+head,4,2,pink)
  if kind=='eat':box(29,43+head,6,3+(index%2)*2,ink)
 if kind=='eat':box(38+index%2,46,8,5,cream);box(39+index%2,45,6,2,pink)
 # Stage markers/accessories make all four provisional forms visibly distinct.
 if stage>=2:box(25,9,15,3,ink);box(28,6,9,3,body)
 if stage>=3:box(5,43,4,10,pink);box(55,43,4,10,pink)
 if stage>=4:box(24,4,17,2,cream);box(27,1,3,4,cream);box(35,1,3,4,cream)
 for dot in range(stage):box(23+dot*5,61,3,2,body)
 return p
for species in SPECIES:
 for stage in range(1,5):
  rel=Path('assets/characters')/species/f'stage_{stage:02d}'/'placeholder';out=ROOT/rel
  lines=['petanim 1','# All frames are provisional, no reference supplied.']
  for kind,(count,fps,loop) in CLIPS.items():
   cells=[image(species,stage,kind,i) for i in range(count)];atlas=bytearray(64*count*64*4)
   for y in range(64):
    for i,cell in enumerate(cells):atlas[(y*count*64+i*64)*4:(y*count*64+(i+1)*64)*4]=cell[y*256:(y+1)*256]
   base.png(out/f'{kind}.png',64*count,64,atlas);lines.append(f'clip {kind} {fps} {int(loop)}')
   for i in range(count):
    dy=[0,-3,-7,-8,-4,0][i] if kind=='happy' else [0,-1,0,-1][i%4] if kind in ('idle','walk','sleep') else 0
    lines.append(f'frame "{(rel/(kind+".png")).as_posix()}" {i*64} 0 64 64 0 {dy}')
   lines.append('end')
  (out/'pet.anim').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('Generated 20 clearly provisional forms, six clips each.')
