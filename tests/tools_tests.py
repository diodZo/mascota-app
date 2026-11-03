from pathlib import Path
import subprocess,tempfile,json
from PIL import Image
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(dir=root/'build',prefix='prepare-test-') as folder:
 folder=Path(folder)
 command=['python',str(root/'tools/asset_validator/prepare_reference.py'),'--input',str(root/'assets/characters/pato_conejo/baby/placeholder/individual/happy_0.png'),'--output',str(folder/'candidate.png'),'--crop','0','0','64','64','--confirm-isolated-character']
 subprocess.run(command,check=True,capture_output=True)
 with Image.open(folder/'candidate.png') as candidate:assert candidate.size==(64,64) and candidate.mode=='RGBA'
 assert json.loads((folder/'candidate.review.json').read_text())['production_approved'] is False
 Image.new('RGBA',(64,64),(1,2,3,255)).save(folder/'opaque.png')
 command[command.index('--input')+1]=str(folder/'opaque.png')
 result=subprocess.run(command,capture_output=True,text=True)
 assert result.returncode!=0 and 'no transparency' in result.stderr
 print('PASS: preparation candidate RGBA64, review report, opaque-background rejection')
# Asset checks independent of current directory.
subprocess.run([str(root/'build/assets_tests.exe'),str(root)],cwd=Path.cwd(),check=True)
