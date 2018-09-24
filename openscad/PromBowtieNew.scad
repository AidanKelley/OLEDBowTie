widthAll = 54;
finalDAll = 64;
endWidthAll = 10;
heightTop = 10;
heightBottom = 6;
w = 1.4;
firstDAll = 27.4+2*w;

module ice(width, finalD, firstD, endWidth, height, heightLower, firstHeightLower, top=true) {
	
		hull() {
			if(top) {
				difference() {
					translate([width - endWidth, 0, heightLower]) scale([endWidth, finalD/2, height]) sphere(r=1, $fn=48);
					translate([0, -50, -50]) cube([100, 100, 50]);
				}
			}
			translate([width-endWidth, 0, 0]) scale([endWidth, finalD/2, heightLower]) cylinder(r=1, h=1, $fn=48);
			translate([0, -firstD/2, 0]) cube([1, firstD, firstHeightLower]);
		}
}

module arm(indent) {
	difference() {
		difference() {
			ice(widthAll, finalDAll, firstDAll, endWidthAll, heightTop, heightBottom, heightBottom);
			difference() {
				translate([w, 0, -0.1]) ice(widthAll - 2*w, finalDAll - 2*w, firstDAll - 2 * w, endWidthAll - w, heightTop - w, heightBottom+0.1, heightBottom-w+0.1);
				translate([0, -50, -0.2]) cube([indent, 100, 50]);
			}
		}
		intersection() {
			translate([-10, -56, -0.1]) rotate([0, 0, 0])
            for(x = [-1:1:12]) {
				for(y = [-1:1:12]) {
					translate([4*x * sqrt(3), 8*(y + 0.5 * x), 0]) if(x%2==0 && y%2==0) {
                        cylinder(d=9, h=70, $fn=24);
                    }
                    else {
                        cylinder(d=3, h=70, $fn=24);
                    }
				}
			}
			difference() {
				translate([w, 0, -0.1]) ice(widthAll - 2*w, finalDAll - 2*w, firstDAll - 2 * w, endWidthAll - w, heightTop - w, 50, 50, false);
				//translate([0, -50, -0.2]) cube([indent, 100, 50]);
			}
		}
	}
}

module display() {
		translate([0, 0, -5]) hull() {
			for(x = [1.4, 26]) {
				for(y = [1.4, 26]) {
					translate([x, y, 0]) cylinder(r=1.4, h=2.2+5, $fn=24);
				}
			}
		}
		//for(y = [9.89:2.54:9.89+3*2.54+0.1]) {
			//translate([1.6, y, 2.2]) cylinder(d=1.8, h=31, $fn=16);
		//}
		translate([(27.4 - 19.6)/2, 0, 2.119]) cube([19.6, 27.4, 1.401]);
		translate([(27.4+19.6)/2-0.01, (27.4-12.4)/2, -5]) cube([(27.4-19.6)/2+5, 12.4, 8.6]);
		translate([6.5, (27.4-23.6)/2, 3.3]) cube([12, 23.6, 5]);
}
module center() {
	cube([12 + 5.2, firstDAll, heightBottom]);
	for(y = [0, firstDAll]) {
		translate([17.2/2, y, 0]) scale([1.0, 0.15, 1.0]) cylinder(d=17.2, h=heightBottom, $fn=24);
	}
}

module encoderHole() {
	cylinder(d1=4.4, d2=4, h=0.4, $fn=24);
	cylinder(d=2.4, h=10, $fn=16);
	translate([0, 0, -5]) cylinder(d=4.4, h=5.001, $fn=24);
}

module buttonHole() {
	translate([-3, 0, -0.1]) cube([10, 6.4, 6.1]);
}

//display();

module wireCutout() {
	ww = 2;
	slope = (finalDAll - firstDAll) / (widthAll - endWidthAll) / 2;
	linear_extrude(5) polygon(points=[
		[0, 0],
		[0, ww],
		[10, ww+slope*10],
		[10, slope*10]
	]);
}


translate([0, 0, 0]) difference() {
	union() {
		translate([0, -firstDAll/2, 0]) center();
		translate([0, 0, 0]) mirror([1, 0, 0]) arm(indent=6.5-2.6+w);
		translate([17.2, 0, 0]) arm(27.4-17.2-6.5+2.6+w);
	}
	translate([-6.5 + 2.6, w - firstDAll/2, 1.8]) display();
	translate([-widthAll+w, 0, 4.5]) rotate([0, -90, 0]) encoderHole();
	for(i = [0, 1]) {
		mirror([0, i, 0]) translate([widthAll+17.2-2*w, 10.3, 0.5]) buttonHole();
	}
	mirror([0, 1, 0]) translate([17.2, -firstDAll/2 + 3.84, -0.1]) mirror([0, 1, 0]) wireCutout();
	mirror([0, 1, 0]) translate([0, -firstDAll/2 + 3.84, -0.1]) mirror([1, 0, 0]) mirror([0, 1, 0]) wireCutout();
}



