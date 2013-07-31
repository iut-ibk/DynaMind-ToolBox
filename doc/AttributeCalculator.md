The module allows calculation of new or overwriting of existing attributes. 

##Parameter
|        Name       |          Type          |       Description         | 
|-------------------|------------------------|---------------------------|
| View     | COMPONENT | view to be worked on        |
| Attribute| double/string/double vector | name of the attribute to write the result to. If an existing attribute results existing value is overwritten,  otherwise a new attribute is created       |
|Variables| string map | links variable names to attributes used in the expression|
|Expression| string | expression that is evaluated. The result can be either a double or string value|
|Append result to vector| bool | result is appended to a double vector|


##Datastream
|     Identifier    |     Attribute    |      Type             |Access |    Description    |
|-------------------|------------------|-----------------------|-------|-------------------|
| [View] |          | COMPONENT | read  | view to be worked on |
|                   | (1) Variables | string / double  | read | first attribute defined in Variables |
|                   | (2) Variables | string / double  | read | second attribute defined in Variables |
|                   | (3) Variables  | LINK | read |link to attribute in view linked to to [View] |
|                   | (n) | string / double  | read | n attribute defined in Variables |
|                   | [Attribute] | string / double   | write | result of expression |
|[Linked View]      | |  COMPONENT | read | link to attribute in view linked to to [View] |
|                   | (3) Variables | string / double  | read | third attribute defined in Variables |

#Detailed Description

The module is based on [muparserX](http://code.google.com/p/muparserx/), a parser for mathematical equations. The attribute that is attended or overwritten is defined in the Attribute field. The view to which the attribute is appended / modified can be defined in the drop down box. Variables can be defined by using the graphical user interface. The defined variable name can be used in the expression to calculate the new value. With the option append result to vector the result is appended to a double vector. This option is useful to collect results. 

[[images/AttributeCalculator.png]]

## Linked Data
Linked data can be used as well in the variable editor. For example View1 is linked with View2. In the “Define Variable” editor it looks like following. 

- View1
- Attribute_1
- Attribute_2
- View2
	- Attribute_3
	- Attribute_4
       	
If you create a variable c based on Attribute_3 following line is created. 

View1.View2.Attribute_3 | c

An component within View1 can be linked to multiple objects from type View2. For the variable c the sum of all the linked objects is calculated.

To extend the functionality following variables can be used:

Name  | Function
----- | -------
nov_variable_name  (e.g. nov_c) | returns the number of linked objects e.g.
first_variable_name (e.g. first_c) | returns the value from the first object linked
rand(value) | returns random number in range ]0-value]
round(value) | rounds value to nearest int value
counter | current counter when used in loops

##Example Expressions

- Adds the attribute year with a constant value 2010 to the city. If you want to add a string value just use "value of string" e.g. "2010" would add the year as string

[[images/attribute_ex2.png]]

- The AttributeCalculator is often used to mark object e.g. as selected. In the following example we select all components of the SUPERBLOCK where the ZONE_CODE is "R1Z" with 1 and all others with 0. In mupaserX the layout of the if statement has changed (see [muparserX])(http://code.google.com/p/muparserx/wiki/Introduction#Features)

[[images/attribute_ex4.png]]

- Old style of if statement still supported. 

[[images/attribute_ex3.png]]
