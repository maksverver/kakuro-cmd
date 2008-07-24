#!/usr/bin/perl

# Read dimensions
($rows, $cols) = split /\s+/, <>;

# Generate grid
print '<?xml version="1.0" standalone="no" ?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
    "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
    width="', 20 + 30*$cols, '" height="', 20 + 30*$rows, '" version="1.1">
<desc>Kakuro puzzle</desc>';
print '<rect x="', 10,'" y="', 10,'" width="', 30*$cols,'" height="', 30*$rows,'" stroke="black" fill="none" stroke-width="2" />';
for($row = 1; $row < $rows; ++$row)
{
    print '<line x1="', 10,'" y1="', 10+30*$row,'" x2="', 10+30*$cols,'" y2="', 10+30*$row,'" stroke="black" stroke-width="2" />';
}
for($col = 1; $col < $cols; ++$col)
{
    print '<line x1="', 10+30*$col,'" y1="', 10,'" x2="', 10+30*$col,'" y2="', 10+30*$rows,'" stroke="black" stroke-width="2" />';
}

# Process clues
for($row = 0; $row < $rows; ++$row)
{
    @cells = grep !/^$/, (split/\s+/, <>);

    for($col = 0; $col < $cols; ++$col)
    {
        $cell = $cells[$col];

        if($cell =~ /(.*)\\(.*)/)
        {
            print '<rect x="', 12+30*$col,'" y="', 12+30*$row,'" width="', 26,'" height="', 26,'" stroke="none" fill="black" />';
            print '<line x1="', 12+30*$col,'" y1="', 12+30*$row,'" x2="', 12+26+30*$col,'" y2="', 12+26+30*$row,'" stroke="white" stroke-width="1" />';
            if($1) {
                print '<text x="', 10 + 4 + 30*$col, '" y="', 10 + 25 + 30*$row, '" text-anchor="begin" fill="white" font-family="sans-serif" font-size="10">',            $1,'</text>';
            }
            if($2) {
                print '<text x="', 10 + 26 + 30*$col, '" y="', 10 + 12 + 30*$row, '" text-anchor="end" fill="white" font-family="sans-serif" font-size="10">',            $2,'</text>';
            }
        }
        elsif($cell =~ /\(([1-9](,[1-9])*)\)/)
        {
            @candidates = split /,/, $1;
            foreach $c (@candidates) {
                print '<text x="', 10 + 4 + 8*   (($c-1)%3) + 30*$col,
                          '" y="', 10 + 8 + 8*int(($c-1)/3) + 30*$row,
                       '" font-family="sans-serif" font-size="6" fill="grey">',
                       $c,'</text>';
            }
        }
        elsif($cell =~ /([1-9])/)
        {
            print '<text x="', 10 + 8 + 30*$col, '" y="',   10 + 24 + 30*$row,
                    '" font-family="sans-serif" font-size="24" fill="black">',
                    $cell,'</text>';
        }

    }
}
print '</svg>';
exit;
