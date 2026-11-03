"""Generate clearly provisional assets; no source reference is implied.
Uses only Python's standard library. Re-running produces the same PNG bytes.
"""
from pathlib import Path
import struct,zlib,json
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'assets/characters/pato_conejo/baby/placeholder'
PALETTE={'o':(88,65,91,255),'b':(255,224,151,255),'l':(255,239,188,255),'p':(242,161,173,255),'a':(235,153,77,255),'w':(255,247,218,255)}
def png(path,w,h,data):
 def chunk(k,v): return struct.pack('>I',len(v))+k+v+struct.pack('>I',zlib.crc32(k+v)&0xffffffff)
 raw=b''.join(b'\0'+data[y*w*4:(y+1)*w*4] for y in range(h))
 path.parent.mkdir(parents=True,exist_ok=True)
 path.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,6,0,0,0))+chunk(b'IDAT',zlib.compress(raw,9))+chunk(b'IEND',b''))
def frame(kind,index):
 pixels=bytearray(64*64*4)
 def box(x,y,w,h,c):
  for yy in range(max(0,y),min(64,y+h)):
   for xx in range(max(0,x),min(64,x+w)):
    pixels[(yy*64+xx)*4:(yy*64+xx+1)*4]=bytes(PALETTE[c])
 # Same silhouette and palette throughout; only limbs, head and face vary.
 head=(1 if index==0 else 2) if kind=='sad' else (index%2) if kind=='sleep' else -1 if kind=='idle' and index in (5,6,7) else 0
 for x in (16,38):
  box(x,4+head,10,26,'o');box(x+2,6+head,6,22,'b');box(x+3,8+head,4,16,'p')
 box(12,27+head,40,28,'o');box(8,34+head,48,15,'o')
 box(14,29+head,36,24,'b');box(10,36+head,44,11,'b');box(17,30+head,28,5,'l')
 # Small duck wings; not a new final character design.
 wing= -1 if kind=='happy' and index in (1,2,3) else 0
 for x in (7,49):
  box(x,40+wing,8,10,'o');box(x+2,41+wing,4,6,'w')
 left,right=(0,0)
 if kind=='walk': left,right=((0,-2),(-2,0),(0,2),(2,0))[index%4]
 box(16,54+left,12,5,'o');box(18,54+left,8,3,'a')
 box(36,54+right,12,5,'o');box(38,54+right,8,3,'a')
 closed=(kind=='idle' and index==14) or kind in ('happy','sleep')
 for x in (20,40):
  if kind=='sad':box(x,36+head,4,2,'o');box(x+(1 if x==20 else -1),34+head,3,1,'o')
  elif closed:box(x,36+head,5,2,'o');box(x+1,35+head,3,1,'o')
  else:box(x,34+head,4,5,'o');box(x+1,34+head,1,1,'w')
 box(27,40+head,12,7,'a');box(28,40+head,10,2,'l')
 if kind=='happy':box(30,45+head,6,2,'o');box(31,47+head,4,1,'p')
 if kind=='sad':box(30,44+head,6,1,'o');box(29,45+head,2,1,'o');box(35,45+head,2,1,'o')
 if kind=='idle' and index in (6,7,8):box(30,45+head,5,1,'o')
 if kind=='sleep':box(29,45+head,4,1,'o')
 box(16,41+head,5,3,'p');box(44,41+head,5,3,'p')
 return pixels
spec={'idle':(16,4,True),'happy':(6,8,False),'walk':(4,8,True),'sad':(3,2,True),'sleep':(2,1,True)}
lines=['petanim 1','# PROVISIONAL: no original reference available']
meta={'schema_version':2,'placeholder':True,'frame_width':64,'frame_height':64,'anchor':[32,60],'runtime_manifest':'assets/characters/pato_conejo/baby/placeholder/pet.anim','animations':{}}
for kind,(count,fps,loop) in spec.items():
 cells=[frame(kind,i) for i in range(count)]
 atlas=bytearray(64*count*64*4)
 for y in range(64):
  for i,cell in enumerate(cells):atlas[(y*count*64+i*64)*4:(y*count*64+(i+1)*64)*4]=cell[y*256:(y+1)*256]
 png(OUT/f'{kind}.png',64*count,64,atlas)
 lines.append(f'clip {kind} {fps} {int(loop)}')
 for i,cell in enumerate(cells):
  dy=([0,-1,-2,-1][(i//2)%4] if kind=='idle' else [0,-3,-7,-8,-4,0][i] if kind=='happy' else [0,-1,0,-1][i] if kind=='walk' else [0,1,1][i] if kind=='sad' else [0,-1][i])
  lines.append(f'frame "assets/characters/pato_conejo/baby/placeholder/{kind}.png" {i*64} 0 64 64 0 {dy}')
  if kind=='happy':png(OUT/'individual'/f'{kind}_{i}.png',64,64,cell)
 lines.append('end')
 meta['animations'][kind]={'frames':count,'fps':fps,'loop':loop,'sheet':f'{kind}.png'}
(OUT/'pet.anim').write_text('\n'.join(lines)+'\n',encoding='utf-8')
# Independent frames manifest exercises the same loader with x=y=0.
individual=lines.copy()
for i,line in enumerate(individual):
 if line.startswith('frame ') and '/happy.png' in line:
  parts=line.split();index=int(parts[2])//64
  individual[i]=f'frame "assets/characters/pato_conejo/baby/placeholder/individual/happy_{index}.png" 0 0 64 64 0 {parts[-1]}'
(OUT/'individual.anim').write_text('\n'.join(individual)+'\n',encoding='utf-8')
(ROOT/'config/animations.json').write_text(json.dumps(meta,indent=2)+'\n',encoding='utf-8')
print('Generated provisional PNG atlases, individual happy frames and manifests.')
