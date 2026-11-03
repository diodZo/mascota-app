"""Supervised preparation only. Does not remove backgrounds or invent frames.
Optional dependency: Pillow. The application does not depend on this tool.
"""
import argparse,json
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--input',type=Path,required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--crop',type=int,nargs=4,metavar=('X','Y','W','H'),required=True)
parser.add_argument('--confirm-isolated-character',action='store_true',help='Confirm the crop contains only the intended character, no text/background/other characters')
args=parser.parse_args()
if not args.confirm_isolated_character:parser.error('Human review required: use --confirm-isolated-character only after inspecting the source')
try:from PIL import Image
except ImportError:parser.error('Optional tool requires Pillow; install it in a separate tools environment')
with Image.open(args.input) as source:
 image=source.convert('RGBA')
x,y,w,h=args.crop
if min(x,y)<0 or min(w,h)<1 or x+w>image.width or y+h>image.height:parser.error('Crop lies outside the reference')
crop=image.crop((x,y,x+w,y+h))
if crop.getchannel('A').getextrema()[0]==255:parser.error('Crop has no transparency: manually isolate the character first; a rectangular crop is not production art')
box=crop.getbbox()
if box is None:parser.error('Empty crop')
crop=crop.crop(box)
scale=min(56/crop.width,56/crop.height)
size=(max(1,round(crop.width*scale)),max(1,round(crop.height*scale)))
prepared=crop.resize(size,Image.Resampling.NEAREST)
canvas=Image.new('RGBA',(64,64));canvas.alpha_composite(prepared,((64-size[0])//2,60-size[1]))
args.output.parent.mkdir(parents=True,exist_ok=True);canvas.save(args.output)
preview=canvas.resize((384,384),Image.Resampling.NEAREST);preview.save(args.output.with_name(args.output.stem+'_preview.png'))
report={'source':str(args.input),'crop':[x,y,w,h],'output':str(args.output),'anchor':[32,60],'production_approved':False,'review_required':['identity and palette','proportions at 64x64','edge halos','feet and wings alignment','silhouette against light and dark backgrounds']}
args.output.with_suffix('.review.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('Prepared review candidate, not production-approved artwork:',args.output)
