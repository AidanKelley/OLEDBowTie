/*
for(x = [2.3, 23.1]) {
	for(y = [2.3]) {
		translate([x, y, 0]) cylinder(d=3.0, h=10.0, $fn=24);
	}
}
*/


module batteryHolder() {
	w = 1.3;
	batt = 10.8;
	inner = 51.6;
	h = 1;
	
	difference() {
		cube([2 * w + 2 * batt, inner + 2*w, h + batt]);
		translate([w, w + 2.3, h+batt/2]) cube([2 * batt, inner - 2.3 - 2.3, batt/2 + 0.1]);
		for(i = [w+batt/2, w+3*batt/2]) {
			translate([i, 2.3+w, h + batt/2]) rotate([-90, 0, 0]) cylinder(d=batt, h=inner - 2.3 - 2.3, $fn=48); 
		}
		for(i = [w, inner + w - 1.0]) {
			translate([w, i, h+0.9]) cube([2*batt, 1.0, 10]);
		}
	
		translate([w + 2, w + 0.9, h+0.9]) cube([6.8, 1.401, 100]);
		translate([w + batt + 3.2, w + 0.9, h + 0.9]) cube([4.4, 1.401, 100]);
		
		translate([w + batt + 2, inner - 1.001, h+0.9]) cube([6.8, 1.401, 100]);
		translate([w + 3.2, inner - 1.001, h + 0.9]) cube([4.4, 1.401, 100]);
		
		
		
	}
	
	translate([w + batt - w/2, 0, 0]) cube([w, 2*w, h+batt]);
}

batteryHolder();