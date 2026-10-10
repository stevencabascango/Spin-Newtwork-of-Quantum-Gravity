const fs=require('fs'),vm=require('vm'),assert=require('assert');
const elements=new Map();function el(id){if(!elements.has(id))elements.set(id,{textContent:'',value:id==='preset'?'lattice':'2',selectedOptions:[{textContent:'Red triangular'}],style:{},listeners:{},addEventListener(type,handler){this.listeners[type]=handler;},getBoundingClientRect(){return {width:600,height:680,left:0,top:0}},getContext(){return new Proxy({},{get(){return ()=>{}}})},setPointerCapture(){}});return elements.get(id);}
const context={document:{getElementById:el,querySelectorAll:()=>[]},window:{devicePixelRatio:1},ResizeObserver:class{observe(){}},requestAnimationFrame(){},console};vm.createContext(context);vm.runInContext(fs.readFileSync('site/dist/app.js','utf8'),context);
vm.runInContext(`
function check(v,message){if(!v)throw Error(message);}
const first=nodes[0],originalX=first.x,originalY=first.y;
canvas.listeners.pointerdown({button:0,pointerId:1,clientX:w/2+(first.x+10)*scale(),clientY:h/2+first.y*scale()});
canvas.listeners.pointermove({clientX:w/2+(originalX+40)*scale(),clientY:h/2+(originalY+20)*scale()});
canvas.listeners.pointerup();
check(Math.abs(first.x-originalX-30)<1e-8&&Math.abs(first.y-originalY-20)<1e-8,'drag preserves grab offset');
check(drag===null,'drag released');
check(metrics().v===20,'initial vertices');check(metrics().c===1,'connected lattice');
$('preset').value='empty';preset();check(metrics().v===0&&metrics().c===0,'empty');
const a=add(0,0),b=add(90,0),c=add(45,80);connect(a,b);connect(b,c);connect(c,a);snapshot('triangle');check(metrics().b===1&&metrics().chi===0,'triangle');
check(!connect(a,a)&&!connect(a,b)&&!connect(a,999),'invalid edges');
node(a).pinned=true;const x=node(a).x,y=node(a).y;for(let i=0;i<1200;i++)step(1/120);check(node(a).x===x&&node(a).y===y,'pin');check(nodes.every(n=>Number.isFinite(n.x)&&Number.isFinite(n.vx)),'finite integration');
restore(cursor-1);check(metrics().v===0,'undo');check($('message').textContent.includes(states[cursor].operation),'restored state message');restore(cursor+1);check(metrics().v===3,'redo');restore(cursor-1);add(0,0);snapshot('branch');check(cursor===states.length-1&&metrics().v===1,'branch truncation');
for(let i=0;i<110;i++)snapshot('bounded');check(states.length===100&&cursor===99,'history limit');
$('preset').value='tetra';preset();check(metrics().v===4&&metrics().e===6&&metrics().b===3,'K4');
$('preset').value='separate';preset();check(metrics().c===2&&metrics().b===2,'two components');
`,context);console.log('PASS: web presets, invalid edges, metrics, physics, pinning, undo/redo, branching and history limit.');
