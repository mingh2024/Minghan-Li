package main;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

class Expense {
    private static final DateTimeFormatter DATE_FORMAT = DateTimeFormatter.ISO_LOCAL_DATE;
    private LocalDate date;
    private String description;
    private double amount;

    
    public Expense(LocalDate date, String description, double amount) {
        this.date = date;
        this.description = description;
        this.amount = amount;
    }
    

    public LocalDate getDate() {
        return date;
    }
    

    public String getDateString() {
        return date.format(DATE_FORMAT);
    }

    
    public String getDescription() {
        return description;
    }
    

    public double getAmount() {
        return amount;
    }

    
}
