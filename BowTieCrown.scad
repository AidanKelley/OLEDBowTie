difference() {
	union() {
		cylinder(d1=5, d2=6, h=0.5, $fn=24);
		translate([0, 0, 0.5]) cylinder(d=6, h=3, $fn=24);
	}
	
	translate([-1, -1, 1]) cube([2, 2, 10]);
}
