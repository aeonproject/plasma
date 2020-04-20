#!/usr/bin/perl -w
# 
# scopedtimes.pl : part of the Mace toolkit for building distributed systems
# 
# Copyright (c) 2011, Charles Killian, Dejan Kostic, Ryan Braud, James W. Anderson, John Fisher-Ogden, Calvin Hubble, Duy Nguyen, Justin Burke, David Oppenheimer, Amin Vahdat, Adolfo Rodriguez, Sooraj Bhat
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the names of the contributors, nor their associated universities 
#      or organizations may be used to endorse or promote products derived from
#      this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# ----END-OF-LEGAL-STUFF----

use strict;
use Getopt::Long;

my $nosummary = 0;
my $progress = 0;
my $log = 0;
my $help = 0;
my $scopedTimesOnly = 0;
my $rawLog = 0;

GetOptions("progress" => \$progress,
	   "summary" => \$nosummary,
	   "log" => \$log,
	   "rawlog" => \$rawLog,
	   "timesonly" => \$scopedTimesOnly,
	   "help" => \$help);

if ($help) {
    print qq{usage: $0 [options]
  -p print progress
  -s do not print summary
  -l print transition timing log
  -t filter for scoped times only
};
    exit(0);
}

if ($rawLog) {
    $log = 1;
}

if ($log) {
    $progress = 0;
}

my %stack = ();
my %stats = ();

my @logstack = ();

my $indent = 0;
my $lcount = 0;
while (my $l = <>) {
    chomp($l);
    unless ($l =~ m/(STARTING|ENDING)/) {
	next;
    }
    if ($scopedTimesOnly && $l !~ m|ScopedLogTimes|) {
	next;
    }
    if ($l =~ m|ServiceStack|) {
	next;
    }
    $lcount++;
    if ($progress) {
	print STDERR "\r$lcount";
    }
    
    my @a = split(/ /, $l);
    my $sc = scalar(@a);
#     print "split $sc @a\n";
    
    my $time = 0;
    my $sel = "";
    my $bound = "";
    if (scalar(@a) == 3) {
	$time = shift(@a);
	$sel = shift(@a);
	$bound = shift(@a);
    }
    if (scalar(@a) == 4) {
	$time = shift(@a);
	shift(@a); # thread id
	$sel = shift(@a);
	$bound = shift(@a);
    }
    if (scalar(@a) == 5) {
	# needed to support log files generated by LOG_SCOPED_TIMES with
	# multiple selectors selected
	$time = shift(@a);
	shift(@a); # thread id
	shift(@a); # ignore bogus selector
	$sel = shift(@a);
	$bound = shift(@a);
    }

    if ($bound eq "STARTING") {
	push(@{$stack{$sel}}, $time);
	$indent++;
    }
    elsif ($bound eq "ENDING") {
	my $start = pop(@{$stack{$sel}});
	$indent--;
	my $diff = $time - $start;
	if ($diff < 0) {
	    print STDERR "\nignoring $sel with time $diff\n";
	    next;
	}
	if ($diff > 1) {
	    print STDERR "\nfound transitions with time $diff: $sel\n";
	    next;
	}
		
	my $s = { };
	if (!defined($stats{$sel})) {
	    $stats{$sel} = $s;
	}
	if ($log) {
	    my $strf = [$sel, $diff, $indent, $start];
# 	    if (scalar(@logstack) && $indent == $logstack[0]->[2]) {
# 		push(@logstack, $strf);
# 	    }
# 	    else {
# 		unshift(@logstack, $strf);
# 	    }
	    unshift(@logstack, $strf);
	    if ($indent == 0) {
		@logstack = sort { $a->[3] <=> $b->[3] } @logstack;
		while (my $el = shift(@logstack)) {
		    my ($es, $ed, $ei, $ts) = @$el;
		    my $esub = 0;
		    for my $ev (@logstack) {
			if ($$ev[2] - 1 == $ei) {
			    $esub += $$ev[1];
			}
			elsif ($$ev[2] <= $ei) {
			    last;
			}
		    }
		    if ($rawLog) {
			printf("%f %7d %s\n", $ts, $ed * 1000000, $es);
		    }
		    else {
			my $spaces = " " x ($ei * 2);
			printf("%7d %7d %s%s\n", $ed * 1000000,
			       ($ed - $esub) * 1000000,
			       $spaces, $es);
		    }
		}
	    }
	}
	$s = $stats{$sel};
	$s->{cum} += $diff;
	$s->{count}++;
	if (!defined($s->{min}) || $diff < $s->{min}) {
	    $s->{min} = $diff;
	}
	if (!defined($s->{max}) || $diff > $s->{max}) {
	    $s->{max} = $diff;
	}
    }
    else {
	warn "ignoring unknown bound $bound\n";
    }
}

if ($log) {
    print "\n";
}
if ($progress) {
    print STDERR "\r\n";
}

if (!$nosummary) {
    my ($sel, $total, $count, $min, $max, $avg);

    format STDOUT=
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< total: @###.###### count: @###### min: @##### max: @###### avg: @######
$sel,                                                                           $total,           $count,      $min,        $max,        $avg
.




    for my $k (sort { $stats{$b}->{cum} <=> $stats{$a}->{cum} } keys(%stats)) {
	my $s = $stats{$k};
	$sel = $k;
	$total = $s->{cum};
	$count = $s->{count};
	$min = $s->{min} * 1000000;
	$max = $s->{max} * 1000000;
	$avg = $total * 1000000 / $count;
	write();
    }
}
